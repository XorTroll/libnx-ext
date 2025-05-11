#include "ld/ld.h"
#include "ld/ldro.h"
#include "ld/result.h"
#include <elf.h>
#include <string.h>
#include <limits.h>

typedef enum {
    LdModuleState_Invalid,
    LdModuleState_Loaded,
    LdModuleState_Resolved,
    LdModuleState_Initialized,
    LdModuleState_Finalized
} LdModuleState;

typedef struct {
    char name[0x20];
    void *base_addr;
    void *nro_buf;
    void *nrr_buf;
    void *bss_buf;
    bool is_nro;
    bool do_init_fini_array;
} LdModuleInput;

typedef void(*LdModuleInitFiniFn)(void);

typedef struct LdModule LdModule;

struct LdModule {
    LdModuleState state;
    int ref_count;
    LdModuleInput input;
    
    union {
        Elf64_Rel *rel;
        Elf64_Rela *rela;
        void *raw;
    } rela_or_rel_plt;
    union {
        Elf64_Rel *rel;
        Elf64_Rela *rela;
    } rela_or_rel;
    Elf64_Relr *relr;
    Elf64_Dyn *dynamic;
    bool is_rela;
    Elf64_Xword rela_or_rel_plt_size;
    LdModuleInitFiniFn init;
    LdModuleInitFiniFn *init_array;
    Elf64_Xword init_array_size;
    LdModuleInitFiniFn fini;
    LdModuleInitFiniFn *fini_array;
    Elf64_Xword fini_array_size;
    uint32_t *hash_bucket;
    uint32_t *hash_chain;
    char *dynstr;
    Elf64_Sym *dynsym;
    Elf64_Xword dynstr_size;
    void **got;
    Elf64_Xword rela_dyn_size;
    Elf64_Xword rel_dyn_size;
    Elf64_Xword relr_size;
    Elf64_Xword rel_count;
    Elf64_Xword rela_count;
    Elf64_Xword hash_nchain_value;
    Elf64_Xword hash_nbucket_value;
    void *got_stub_ptr;
};

static bool g_Initialized = false;
static LdModule **g_ModuleList = NULL;
static u32 g_MaxModuleCount = 0;
static u32 g_ModuleCount = 0;

extern int main(int argc, char **argv);

NX_INLINE uintptr_t _ldGetMainAslrBase(void) {
    MemoryInfo main_info;
    memset(&main_info, 0, sizeof(main_info));
    u32 page_info = 0;

    svcQueryMemory(&main_info, &page_info, (uintptr_t)&main);

    return main_info.addr;
}

static Result _ldPrepareNroModule(LdModule *mod, void *nro_buf, size_t nro_size) {
    // Create a fake NRR on-the-fly
    size_t nrr_size = 0x1000;
    void *nrr_buf = __libnx_aligned_alloc(EXT_PAGE_ALIGN, nrr_size);
    if(!nrr_buf) {
        return MAKERESULT(Module_Ld, LdResult_InvalidInputNro);
    }
    memset(nrr_buf, 0, nrr_size);
    
    LdNrrHeader *nrr_header = (LdNrrHeader*)nrr_buf;
    nrr_header->magic_nrr0 = LD_NRR0_MAGIC;
    nrr_header->kind = LdNrrKind_User;
    nrr_header->size = nrr_size;
    nrr_header->hash_list_offset = LD_NRR_FIXED_HASH_LIST_OFFSET;
    nrr_header->hash_count = 1;

    // TODO: how to do this on earlier firmware?
    u64 program_id;
    EXT_R_TRY(svcGetInfo(&program_id, InfoType_ProgramId, CUR_PROCESS_HANDLE, 0));
    nrr_header->program_id = program_id;

    u8 *nrr_nro_hash_ptr = (u8*)nrr_buf + LD_NRR_FIXED_HASH_LIST_OFFSET;
    sha256CalculateHash(nrr_nro_hash_ptr, nro_buf, nro_size);

    NroHeader *nro_header = (NroHeader*)((u8*)nro_buf + sizeof(NroStart));
    size_t bss_size = nro_header->bss_size;
    void *bss_buf = __libnx_aligned_alloc(EXT_PAGE_ALIGN, bss_size);
    if(!bss_buf) {
        __libnx_free(nrr_buf);
        return MAKERESULT(Module_Ld, LdResult_InvalidInputNro);
    }
    memset(bss_buf, 0, bss_size);

    uintptr_t nro_addr;
    Result rc = ldroRegisterModuleInfo((uintptr_t)nrr_buf, nrr_size);
    if(R_FAILED(rc)) {
        __libnx_free(nrr_buf);
        __libnx_free(bss_buf);
        return rc;
    }

    rc = ldroMapManualLoadModuleMemory(&nro_addr, (uintptr_t)nro_buf, nro_size, (uintptr_t)bss_buf, bss_size);
    if(R_FAILED(rc)) {
        __libnx_free(nrr_buf);
        __libnx_free(bss_buf);
        return rc;
    }

    mod->input.nro_buf = nro_buf;
    mod->input.nrr_buf = nrr_buf;
    mod->input.bss_buf = bss_buf;
    mod->input.base_addr = (void*)nro_addr;
    extLogf("[ld] NRO prepared at %p", mod->input.base_addr);
    return EXT_R_SUCCESS;
}

NX_INLINE LdModuleHeader *_ldGetModuleHeader(LdModule *mod) {
    // TODO: cleaner way to do this?
    u8 *base_addr8 = (u8*)mod->input.base_addr;
    u32 mod0_offset = *(u32*)&(base_addr8)[4];
    return (LdModuleHeader*)&base_addr8[mod0_offset];
}

// oss-rtld

static Result _ldLoadModule(LdModule *mod) {
    extLogf("[ld] Loading module '%s'...", mod->input.name);
    LdModuleHeader *mod_header = _ldGetModuleHeader(mod);
    if(mod_header->magic_mod0 != LD_MOD0_MAGIC) {
        return MAKERESULT(Module_Ld, LdResult_InvalidInputNro);
    }

    Elf64_Dyn *dynamic = (Elf64_Dyn*)((u8*)mod_header + mod_header->dynamic);
    u8 *aslr_base_addr = (u8*)mod->input.base_addr;
    void *rel_plt = NULL;
    for(; dynamic->d_tag != DT_NULL; dynamic++) {
        switch(dynamic->d_tag) {
            case DT_PLTRELSZ: {
                mod->rela_or_rel_plt_size = dynamic->d_un.d_val;
                break;
            }

            case DT_PLTGOT: {
                mod->got = (void**)(aslr_base_addr + dynamic->d_un.d_val);
                break;
            }

            case DT_HASH: {
                u32 *hash_table = (u32*)(aslr_base_addr + dynamic->d_un.d_val);
                const u32 nbucket = hash_table[0];
                const u32 nchain = hash_table[1];
                mod->hash_nbucket_value = nbucket;
                mod->hash_nchain_value = nchain;
                mod->hash_bucket = &hash_table[2];
                mod->hash_chain = &hash_table[2 + nbucket];
                break;
            }

            case DT_STRTAB: {
                mod->dynstr = (char*)(aslr_base_addr + dynamic->d_un.d_val);
                break;
            }

            case DT_SYMTAB: {
                mod->dynsym = (Elf64_Sym*)(aslr_base_addr + dynamic->d_un.d_val);
                break;
            }

            case DT_REL: {
                mod->rela_or_rel.rel = (Elf64_Rel*)(aslr_base_addr + dynamic->d_un.d_val);
                break;
            }
            case DT_RELSZ: {
                mod->rel_dyn_size = dynamic->d_un.d_val;
                break;
            }
            case DT_RELENT: {
                if(dynamic->d_un.d_val != sizeof(Elf64_Rel)) {
                    return MAKERESULT(Module_Ld, LdResult_InvalidRelSize);
                }
                break;
            }

            case DT_RELA: {
                mod->rela_or_rel.rela = (Elf64_Rela*)(aslr_base_addr + dynamic->d_un.d_val);
                break;
            }
            case DT_RELASZ: {
                mod->rela_dyn_size = dynamic->d_un.d_val;
                break;
            }
            case DT_RELAENT: {
                if(dynamic->d_un.d_val != sizeof(Elf64_Rela)) {
                    return MAKERESULT(Module_Ld, LdResult_InvalidRelaSize);
                }
                break;
            }

            case DT_SYMENT: {
                if(dynamic->d_un.d_val != sizeof(Elf64_Sym)) {
                    return MAKERESULT(Module_Ld, LdResult_InvalidSymSize);
                }
                break;
            }

            case DT_STRSZ: {
                mod->dynstr_size = dynamic->d_un.d_val;
                break;
            }

            case DT_RELR: {
                mod->relr = (Elf64_Relr*)(aslr_base_addr + dynamic->d_un.d_val);
                break;
            }
            case DT_RELRSZ: {
                mod->relr_size = dynamic->d_un.d_val;
                break;
            }
            case DT_RELRENT: {
                if(dynamic->d_un.d_val != sizeof(Elf64_Relr)) {
                    return MAKERESULT(Module_Ld, LdResult_InvalidRelrSize);
                }
                break;
            }

            case DT_INIT: {
                mod->init = (LdModuleInitFiniFn)(aslr_base_addr + dynamic->d_un.d_val);
                break;
            }
            case DT_INIT_ARRAY: {
                mod->init_array = (LdModuleInitFiniFn*)(aslr_base_addr + dynamic->d_un.d_val);
                break;
            }
            case DT_INIT_ARRAYSZ: {
                mod->init_array_size = dynamic->d_un.d_val;
                break;
            }

            case DT_FINI: {
                mod->fini = (LdModuleInitFiniFn)(aslr_base_addr + dynamic->d_un.d_val);
                break;
            }
            case DT_FINI_ARRAY: {
                mod->fini_array = (LdModuleInitFiniFn*)(aslr_base_addr + dynamic->d_un.d_val);
                break;
            }
            case DT_FINI_ARRAYSZ: {
                mod->fini_array_size = dynamic->d_un.d_val;
                break;
            }

            case DT_PLTREL: {
                Elf64_Xword value = dynamic->d_un.d_val;
                mod->is_rela = (value == DT_RELA);
                if((value != DT_REL) && (value != DT_RELA)) {
                    return MAKERESULT(Module_Ld, LdResult_InvalidPltRel);
                }
                break;
            }

            case DT_JMPREL: {
                rel_plt = (void *)(aslr_base_addr + dynamic->d_un.d_val);
                break;
            }

            case DT_RELACOUNT:
                mod->rela_count = dynamic->d_un.d_val;
                break;

            case DT_RELCOUNT:
                mod->rel_count = dynamic->d_un.d_val;
                break;

            case DT_NEEDED:
            case DT_RPATH:
            case DT_SYMBOLIC:
            case DT_DEBUG:
            case DT_TEXTREL:
            default:
                break;
        }
    }

    mod->rela_or_rel_plt.raw = rel_plt;
    return EXT_R_SUCCESS;
}

static void _ldRelocateModule(LdModule *mod) {
    if(mod->rel_count > 0) {
        for(size_t i = 0; i < mod->rel_count; i++) {
            Elf64_Rel *entry = &mod->rela_or_rel.rel[i];
            switch(ELF64_R_TYPE(entry->r_info)) {
                case R_AARCH64_RELATIVE: {
                    Elf64_Addr *ptr = (Elf64_Addr*)((u8*)mod->input.base_addr + entry->r_offset);
                    *ptr += (Elf64_Addr)mod->input.base_addr;
                    break;
                }
                default:
                    extLogf("[ld] Unhandled REL relocation type: %u", ELF64_R_TYPE(entry->r_info));
                    break;
            }
        }
    }

    if(mod->rela_count > 0) {
        for(size_t i = 0; i < mod->rela_count; i++) {
            Elf64_Rela *entry = &mod->rela_or_rel.rela[i];
            switch(ELF64_R_TYPE(entry->r_info)) {
                case R_AARCH64_RELATIVE: {
                    Elf64_Addr *ptr = (Elf64_Addr*)((u8*)mod->input.base_addr + entry->r_offset);
                    *ptr = (Elf64_Addr)mod->input.base_addr + entry->r_addend;
                    break;
                }
                default:
                    extLogf("[ld] Unhandled RELA relocation type: %u", ELF64_R_TYPE(entry->r_info));
                    break;
            }
        }
    }

    if(mod->relr_size > 0) {
        Elf64_Addr *ptr = NULL;
        for(Elf64_Xword index = 0u; index < mod->relr_size / sizeof(Elf64_Relr); index++) {
            Elf64_Relr *entry = &mod->relr[index];

            if((*entry & 1) == 0) {
                ptr = (Elf64_Addr*)((u8*)mod->input.base_addr + *entry);
                *ptr++ += (Elf64_Addr)mod->input.base_addr;
            }
            else {
                u64 bitmap = *entry >> 1;
                while(bitmap) {
                    unsigned id = __builtin_ffsl(bitmap) - 1;
                    bitmap &= ~(1UL << id);
                    ptr[id] += (Elf64_Addr)mod->input.base_addr;
                }
                ptr += CHAR_BIT * sizeof(Elf64_Relr) - 1;
            }
        }
    }
}

static u64 _ldElfHashString(const char *name) {
    u64 h = 0;
    u64 g;

    while(*name) {
        h = (h << 4) + *name++;
        if ((g = h & 0xf0000000)) h ^= g >> 24;
        h &= ~g;
    }
    return h;
}

static Elf64_Sym *_ldGetModuleSymbolByName(LdModule *mod, const char *name) {
    u64 name_hash = _ldElfHashString(name);

    for(u32 i = mod->hash_bucket[name_hash % mod->hash_nbucket_value]; i; i = mod->hash_chain[i]) {
        bool is_common = mod->dynsym[i].st_shndx ? (mod->dynsym[i].st_shndx == SHN_COMMON) : true;
        if(!is_common && (strcmp(name, mod->dynstr + mod->dynsym[i].st_name) == 0)) {
            return &mod->dynsym[i];
        }
    }

    return NULL;
}

static Elf64_Addr _ldLookupGlobalSymbolAddress(const char *name, bool search_in_main) {
    for(u32 i = 0; i < g_ModuleCount; i++) {
        LdModule *mod = g_ModuleList[i];
        if(!search_in_main && !mod->input.is_nro) {
            continue;
        }

        if((mod->state == LdModuleState_Loaded) || (mod->state == LdModuleState_Resolved) || (mod->state == LdModuleState_Initialized)) {
            Elf64_Sym *symbol = _ldGetModuleSymbolByName(mod, name);
            if(symbol && ELF64_ST_BIND(symbol->st_info)) {
                return (Elf64_Addr)mod->input.base_addr + symbol->st_value;
            }
        }
    }
    return 0;
}

static bool _ldTryResolveModuleSymbol(LdModule *mod, Elf64_Addr *target_symbol_address, Elf64_Sym *symbol) {
    const char *name = &mod->dynstr[symbol->st_name];
    if(ELF64_ST_VISIBILITY(symbol->st_other)) {
        Elf64_Sym *target_symbol = _ldGetModuleSymbolByName(mod, name);
        if(target_symbol) {
            *target_symbol_address =
                (Elf64_Addr)mod->input.base_addr + target_symbol->st_value;
            return true;
        }
        else if((ELF64_ST_BIND(symbol->st_info) & STB_WEAK) == STB_WEAK) {
            *target_symbol_address = 0;
            return true;
        }
    }
    else {
        Elf64_Addr address = _ldLookupGlobalSymbolAddress(name, true);
        if(address != 0) {
            *target_symbol_address = address;
            return true;
        }
    }
    return false;
}

static bool _ldResolveModuleSymbol_RelAbsolute(LdModule *mod, Elf64_Rel *entry) {
    u32 r_type = ELF64_R_TYPE(entry->r_info);
    u32 r_sym = ELF64_R_SYM(entry->r_info);

    if((r_type == R_AARCH64_ABS64) || (r_type == R_AARCH64_GLOB_DAT)) {
        Elf64_Sym *symbol = &mod->dynsym[r_sym];
        Elf64_Addr target_symbol_address;

        if(_ldTryResolveModuleSymbol(mod, &target_symbol_address, symbol)) {
            Elf64_Addr *target = (Elf64_Addr*)((u8*)mod->input.base_addr + entry->r_offset);
            *target += target_symbol_address;
            // extLogf("[ld] Resolved REL symbol (%s): %s", mod->input.name, &mod->dynstr[symbol->st_name]);
        }
        else {
            extLogf("[ld] Unresolved REL symbol (%s): %s", mod->input.name, &mod->dynstr[symbol->st_name]);
            return false;
        }
    }

    return true;
}

static bool _ldResolveModuleSymbol_RelaAbsolute(LdModule *mod, Elf64_Rela *entry) {
    uint32_t r_type = ELF64_R_TYPE(entry->r_info);
    uint32_t r_sym = ELF64_R_SYM(entry->r_info);

    if((r_type == R_AARCH64_ABS64) || (r_type == R_AARCH64_GLOB_DAT)) {
        Elf64_Sym *symbol = &mod->dynsym[r_sym];
        Elf64_Addr target_symbol_address;

        if(_ldTryResolveModuleSymbol(mod, &target_symbol_address, symbol)) {
            Elf64_Addr *target = (Elf64_Addr*)((u8*)mod->input.base_addr + entry->r_offset);
            *target = target_symbol_address + entry->r_addend;
            // extLogf("[ld] Resolved RELA symbol (%s): %s", mod->input.name, &mod->dynstr[symbol->st_name]);
        } 
        else {
            extLogf("[ld] Unresolved RELA symbol: (%s): %s", mod->input.name, &mod->dynstr[symbol->st_name]);
            return false;
        }
    }

    return true;
}

static bool _ldResolveModuleSymbol_RelJumpSlot(LdModule *mod, Elf64_Rel *entry) {
    u32 r_type = ELF64_R_TYPE(entry->r_info);
    u32 r_sym = ELF64_R_SYM(entry->r_info);

    if(r_type == R_AARCH64_JUMP_SLOT) {
        Elf64_Addr *target = (Elf64_Addr*)((u8*)mod->input.base_addr + entry->r_offset);
        Elf64_Addr target_address = (Elf64_Addr)mod->input.base_addr + *target;

        Elf64_Sym *symbol = &mod->dynsym[r_sym];
        Elf64_Addr target_symbol_address;

        if(_ldTryResolveModuleSymbol(mod, &target_symbol_address, symbol)) {
            *target += target_symbol_address;
            // extLogf("[ld] Resolved REL-JUMP-SLOT symbol (%s): %s", mod->input.name, &mod->dynstr[symbol->st_name]);
        }
        else {
            extLogf("[ld] Unresolved REL-JUMP-SLOT symbol: (%s): %s", mod->input.name, &mod->dynstr[symbol->st_name]);
            *target = target_address;
            return false;
        }
    }

    return true;
}

static bool _ldResolveModuleSymbol_RelaJumpSlot(LdModule *mod, Elf64_Rela *entry) {
    u32 r_type = ELF64_R_TYPE(entry->r_info);
    u32 r_sym = ELF64_R_SYM(entry->r_info);

    if(r_type == R_AARCH64_JUMP_SLOT) {
        Elf64_Addr *target = (Elf64_Addr*)((u8*)mod->input.base_addr + entry->r_offset);
        Elf64_Addr target_address = (Elf64_Addr)mod->input.base_addr + *target;
        
        Elf64_Sym *symbol = &mod->dynsym[r_sym];
        Elf64_Addr target_symbol_address;

        if(_ldTryResolveModuleSymbol(mod, &target_symbol_address, symbol)) {
            *target = target_symbol_address + entry->r_addend;
            // extLogf("[ld] Resolved RELA-JUMP-SLOT symbol (%s): %s", mod->input.name, &mod->dynstr[symbol->st_name]);
        }
        else {
            extLogf("[ld] Unresolved RELA-JUMP-SLOT symbol: (%s): %s", mod->input.name, &mod->dynstr[symbol->st_name]);
            *target = target_address;
            return false;
        }
    }

    return true;
}

static bool _ldResolveModuleSymbols(LdModule *mod) {
    for(Elf64_Xword index = mod->rel_count; index < mod->rel_dyn_size / sizeof(Elf64_Rel); index++) {
        Elf64_Rel *entry = &mod->rela_or_rel.rel[index];
        if(!_ldResolveModuleSymbol_RelAbsolute(mod, entry)) {
            return false;
        }
    }

    for(Elf64_Xword index = mod->rela_count; index < mod->rela_dyn_size / sizeof(Elf64_Rela); index++) {
        Elf64_Rela *entry = &mod->rela_or_rel.rela[index];
        if(!_ldResolveModuleSymbol_RelaAbsolute(mod, entry)) {
            return false;
        }
    }

    if(mod->is_rela) {
        if(mod->rela_or_rel_plt_size >= sizeof(Elf64_Rela)) {
            for(Elf64_Xword index = 0u; index < mod->rela_or_rel_plt_size / sizeof(Elf64_Rela); index++) {
                Elf64_Rela *entry = &mod->rela_or_rel_plt.rela[index];
                if(!_ldResolveModuleSymbol_RelaJumpSlot(mod, entry)) {
                    return false;
                }
            }
        }
    }
    else if(mod->rela_or_rel_plt_size >= sizeof(Elf64_Rel)) {
        for(Elf64_Xword index = 0u; index < mod->rela_or_rel_plt_size / sizeof(Elf64_Rel); index++) {
            Elf64_Rel *entry = &mod->rela_or_rel_plt.rel[index];
            if(!_ldResolveModuleSymbol_RelJumpSlot(mod, entry)) {
                return false;
            }
        }
    }

    return true;
}

static void _ldInitializeModule(LdModule *mod) {
    if(!mod->input.is_nro) {
        return;
    }

    if(mod->init) {
        extLogf("[ld] Module '%s' init function -> %p", mod->input.name, mod->init);
        mod->init();
    }

    if(mod->init_array && mod->init_array_size && mod->input.do_init_fini_array) {
        size_t init_array_count = mod->init_array_size / sizeof(LdModuleInitFiniFn);
        for(size_t i = 0; i < init_array_count; i++) {
            // RELR relocations already made init_array ptrs absolute
            LdModuleInitFiniFn init_fn = (LdModuleInitFiniFn)mod->init_array[i];
            extLogf("[ld] Module '%s' initarray function [%d/%d] -> %p", mod->input.name, i+1, init_array_count, init_fn);
            if(init_fn) {
                init_fn();
            }
        }
    }
}

static void _ldFinalizeModule(LdModule *mod) {
    if(!mod->input.is_nro) {
        return;
    }

    if(mod->fini) {
        extLogf("[ld] Module '%s' fini function -> %p", mod->input.name, mod->fini);
        mod->fini();
    }

    if(mod->fini_array && mod->fini_array_size && mod->input.do_init_fini_array) {
        size_t fini_array_count = mod->fini_array_size / sizeof(LdModuleInitFiniFn);
        for(size_t i = 0; i < fini_array_count; i++) {
            LdModuleInitFiniFn fini_fn = (LdModuleInitFiniFn)mod->fini_array[i];
            extLogf("[ld] Module '%s' fini function [%d/%d] -> %p", mod->input.name, i+1, fini_array_count, fini_fn);
            if(fini_fn) {
                fini_fn();
            }
        }
    }
}

static Result _ldEnsureModuleInitialized(LdModule *mod) {
    if(mod->state == LdModuleState_Invalid) {
        return MAKERESULT(Module_Ld, LdResult_InvalidModuleState);
    }
    if(mod->state == LdModuleState_Initialized) {
        // extLogf("[ld] Module '%s' already initialized", mod->input.name);
        return EXT_R_SUCCESS;
    }

    if(mod->state == LdModuleState_Loaded) {
        extLogf("[ld] Resolving module '%s'...", mod->input.name);
        if(_ldResolveModuleSymbols(mod)) {
            mod->state = LdModuleState_Resolved;
        }
        else {
            return MAKERESULT(Module_Ld, LdResult_UnresolvedSymbol);
        }
    }

    if(mod->state == LdModuleState_Resolved) {
        extLogf("[ld] Initializing module '%s'...", mod->input.name);
        _ldInitializeModule(mod);
        mod->state = LdModuleState_Initialized;
    }

    if(mod->state != LdModuleState_Initialized) {
        extLogf("[ld] Module '%s' not correctly initialized, state %d", mod->input.name, mod->state);
        return MAKERESULT(Module_Ld, LdResult_InvalidModuleState);
    }
    return EXT_R_SUCCESS;
}

static Result _ldLoadMainModule() {
    LdModule *main_mod = (LdModule*)__libnx_alloc(sizeof(LdModule));
    if(main_mod == NULL) {
        return MAKERESULT(Module_Ld, LdResult_FailedToInitialize);
    }
    memset(main_mod, 0, sizeof(LdModule));

    uintptr_t aslr_base = _ldGetMainAslrBase();
    if(aslr_base == 0) {
        __libnx_free(main_mod);
        return MAKERESULT(Module_Ld, LdResult_InvalidMainAslrBase);
    }

    main_mod->input.base_addr = (void*)aslr_base;
    strncpy(main_mod->input.name, "main", sizeof(main_mod->input.name));
    main_mod->input.is_nro = false;
    main_mod->state = LdModuleState_Invalid;

    EXT_R_TRY(_ldLoadModule(main_mod));

    main_mod->state = LdModuleState_Loaded;
    g_ModuleList[g_ModuleCount] = main_mod;
    g_ModuleCount++;
    return EXT_R_SUCCESS;
}

Result ldInitialize(u32 max_modules) {
    if(g_Initialized) {
        return EXT_R_SUCCESS;
    }

    g_MaxModuleCount = max_modules;
    g_ModuleList = (LdModule**)__libnx_alloc(sizeof(LdModule*) * g_MaxModuleCount);
    if(g_ModuleList == NULL) {
        return MAKERESULT(Module_Ld, LdResult_FailedToInitialize);
    }
    memset(g_ModuleList, 0, sizeof(LdModule*) * g_MaxModuleCount);

    EXT_R_TRY(ldroInitialize());
    EXT_R_TRY(_ldLoadMainModule());
    g_Initialized = true;
    return EXT_R_SUCCESS;
}

void ldExit(void) {
    if(!g_Initialized) {
        return;
    }

    ldUnloadAllModules();
    ldroExit();
    __libnx_free(g_ModuleList);
    g_ModuleList = NULL;
    g_Initialized = false;
}

Result ldLoadModuleFromNro(void *nro_buf, size_t nro_size, const char *name, bool do_init_fini_array) {
    if(!g_Initialized) {
        return MAKERESULT(Module_Ld, LdResult_NotInitialized);
    }
    if(g_ModuleCount >= g_MaxModuleCount) {
        return MAKERESULT(Module_Ld, LdResult_ModuleLimitReached);
    }
    if((nro_buf == NULL) || (nro_size == 0)) {
        return MAKERESULT(Module_Ld, LdResult_InvalidArgument);
    }
    if(name == NULL) {
        return MAKERESULT(Module_Ld, LdResult_InvalidArgument);
    }

    LdModule *mod = (LdModule*)__libnx_alloc(sizeof(LdModule));
    if(mod == NULL) {
        return MAKERESULT(Module_Ld, LdResult_FailedToInitialize);
    }
    memset(mod, 0, sizeof(LdModule));
    strcpy(mod->input.name, name);
    mod->state = LdModuleState_Invalid;
    mod->input.is_nro = true;
    mod->input.do_init_fini_array = do_init_fini_array;

    EXT_R_TRY(_ldPrepareNroModule(mod, nro_buf, nro_size));
    EXT_R_TRY(_ldLoadModule(mod));
    _ldRelocateModule(mod);

    mod->state = LdModuleState_Loaded;
    g_ModuleList[g_ModuleCount] = mod;
    g_ModuleCount++;
    return EXT_R_SUCCESS;
}

static void _ldDisposeModule(LdModule *mod) {
    if(mod->state == LdModuleState_Initialized) {
        _ldFinalizeModule(mod);
        mod->state = LdModuleState_Finalized;
    }

    if(mod->state != LdModuleState_Invalid) {
        if(mod->input.is_nro) {
            ldroUnmapManualLoadModuleMemory((uintptr_t)mod->input.nro_buf);
            ldroUnregisterModuleInfo((uintptr_t)mod->input.nrr_buf);

            __libnx_free(mod->input.nrr_buf);
            __libnx_free(mod->input.bss_buf);
            __libnx_free(mod->input.nro_buf);
        }
    }
    
    __libnx_free(mod);
}

bool ldUnloadModule(const char *name) {
    if(!g_Initialized) {
        return false;
    }

    for(u32 i = 0; i < g_ModuleCount; i++) {
        LdModule *mod = g_ModuleList[i];
        if(strcmp(mod->input.name, name) == 0) {
            _ldDisposeModule(mod);
            g_ModuleList[i] = NULL;
            return true;
        }
    }

    return false;
}

void ldUnloadAllModules(void) {
    for(u32 i = 0; i < g_ModuleCount; i++) {
        LdModule *mod = g_ModuleList[i];
        if(mod != NULL) {
            _ldDisposeModule(mod);
            g_ModuleList[i] = NULL;
        }
    }
}

Result ldLookupGlobalSymbol(const char *name, bool search_in_main, void **out_sym) {
    for(u32 i = 0; i < g_ModuleCount; i++) {
        LdModule *mod = g_ModuleList[i];
        EXT_R_TRY(_ldEnsureModuleInitialized(mod));
    }

    void *sym = (void*)_ldLookupGlobalSymbolAddress(name, search_in_main);
    if(sym == NULL) {
        return MAKERESULT(Module_Ld, LdResult_SymbolNotFound);
    }
    *out_sym = sym;
    return EXT_R_SUCCESS;
}
