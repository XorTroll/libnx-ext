
#pragma once
#include "ext.h"

#define LD_NRR0_MAGIC 0x3052524E
#define LD_MOD0_MAGIC 0x30444F4D

typedef struct {
    u32 magic_mod0;
    u32 dynamic;
    u32 bss_start;
    u32 bss_end;
    u32 unwind_start;
    u32 unwind_end;
    u32 module_object;

    u32 magic_lny0;
	s32 got_start_offset;
	s32 got_end_offset;

	u32 magic_lny1;
	s32 relro_start_offset;
	s32 relro_end_offset;
} LdModuleHeader;

// https://switchbrew.org/wiki/NRR

typedef enum {
    LdNrrKind_User = 0,
    LdNrrKind_JitPlugin = 1,
} LdNrrKind;

typedef struct {
    u64 program_id_mask;
    u64 program_id_pattern;
    u8 reserved[0x10];
    u8 public_key[0x100];
    u8 signature[0x100];
} LdNrrCertification;
_Static_assert(sizeof(LdNrrCertification) == 0x220, "LdNrrCertification definition");

typedef struct {
    u32 magic_nrr0;
    u8 sign_key_generation;
    u8 reserved[0xB];
    LdNrrCertification certification;
    u8 signature[0x100];
    u64 program_id;
    u32 size;
    u8 kind; // LdNrrKind
    u8 reserved2[0x3];
    u32 hash_list_offset;
    u32 hash_count;
    u8 reserved3[0x8];
} LdNrrHeader;
_Static_assert(sizeof(LdNrrHeader) == 0x350, "LdNrrHeader definition");

#define LD_NRR_FIXED_HASH_LIST_OFFSET sizeof(LdNrrHeader)

Result ldInitialize(u32 max_modules);
void ldExit(void);

Result ldLoadModuleFromNro(void *nro_buf, size_t nro_size, const char *name, bool do_init_fini_array);
bool ldUnloadModule(const char *name);
void ldUnloadAllModules(void);

Result ldLookupGlobalSymbol(const char *name, bool search_in_main, void **out_sym);
