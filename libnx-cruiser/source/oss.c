#include "cruiser/oss.h"
#include "cruiser/browserdll.h"
#include "cruiser/sym.h"
#include "cruiser/result.h"
#include "switch-ld.h"
#include <lz4.h>
#include <stdio.h>

/*

Changes across firmware versions:

start: /dll/{mod}.nro
5.0.0: *.nro.lz4 instead of *.nro
8.0.0: dll renamed to dll_0
9.0.0: new dll_1, now /dll_0/ and /dll_1/ (/dll_0/ for OfflineWeb)
11.0.0: /dll_X/ is now /nro/netfront/dll_X/
12.1.0: dll_X renamed to core_X (core_0 and core_1)
14.0.0: /nro/netfront/core_{0,1}/ are now /nro/netfront/core_{0,2}/default/cfi_{dis,en}enabled/
20.0.0: /nro/netfront/core_{0,2}/default/cfi_{dis,en}enabled/ are now /nro/netfront/core_{0,3}/default/cfi_{dis,en}enabled/

*/

typedef enum {
    OssBrowserDllNro_Libfont,
    OssBrowserDllNro_CairoWkc,
    OssBrowserDllNro_OssWkc,
    OssBrowserDllNro_PeerWkc,
    OssBrowserDllNro_WebKitWkc,

    OssBrowserDllNro_Count
} OssBrowserDllNro;

char g_BrowserDllNroNames[OssBrowserDllNro_Count][64] = {
    "libfont",
    "cairo_wkc",
    "oss_wkc",
    "peer_wkc",
    "webkit_wkc",
};

char g_BrowserDllNroPath_1[64] = "dll";
char g_BrowserDllNroPath_2[64] = "dll_0";
char g_BrowserDllNroPaths_3[OssBrowserDllKind_Count][64] = {
    "dll_0",
    "dll_1",
};
char g_BrowserDllNroPaths_4[OssBrowserDllKind_Count][64] = {
    "nro/netfront/dll_0",
    "nro/netfront/dll_1",
};
char g_BrowserDllNroPaths_5[OssBrowserDllKind_Count][64] = {
    "nro/netfront/core_0",
    "nro/netfront/core_1",
};
char g_BrowserDllNroPaths_6[OssBrowserDllKind_Count][64] = {
    "nro/netfront/core_0/default/cfi_disabled",
    "nro/netfront/core_2/default/cfi_enabled",
};
char g_BrowserDllNroPaths_7[OssBrowserDllKind_Count][64] = {
    "nro/netfront/core_0/default/cfi_disabled",
    "nro/netfront/core_3/default/cfi_enabled",
};

NX_INLINE bool _ossIsBrowserDllNroLz4Compressed(void) {
    return hosversionAtLeast(5,0,0);
}

static void _ossFormatBrowserDllNroPath(char *out_path, size_t out_path_size, OssBrowserDllKind kind, OssBrowserDllNro nro) {
    char nro_name[128];
    snprintf(nro_name, sizeof(nro_name), "%s.%s", g_BrowserDllNroNames[nro], _ossIsBrowserDllNroLz4Compressed() ? "nro.lz4" : "nro");

    if(hosversionAtLeast(20,0,0)) {
        snprintf(out_path, out_path_size, BROWSERDLL_MOUNT_NAME ":/%s/%s", g_BrowserDllNroPaths_7[kind], nro_name);
    }
    else if(hosversionAtLeast(14,0,0)) {
        snprintf(out_path, out_path_size, BROWSERDLL_MOUNT_NAME ":/%s/%s", g_BrowserDllNroPaths_6[kind], nro_name);
    }
    else if(hosversionAtLeast(12,1,0)) {
        snprintf(out_path, out_path_size, BROWSERDLL_MOUNT_NAME ":/%s/%s", g_BrowserDllNroPaths_5[kind], nro_name);
    }
    else if(hosversionAtLeast(11,0,0)) {
        snprintf(out_path, out_path_size, BROWSERDLL_MOUNT_NAME ":/%s/%s", g_BrowserDllNroPaths_4[kind], nro_name);
    }
    else if(hosversionAtLeast(9,0,0)) {
        snprintf(out_path, out_path_size, BROWSERDLL_MOUNT_NAME ":/%s/%s", g_BrowserDllNroPaths_3[kind], nro_name);
    }
    else if(hosversionAtLeast(8,0,0)) {
        snprintf(out_path, out_path_size, BROWSERDLL_MOUNT_NAME ":/%s/%s", g_BrowserDllNroPath_2, nro_name);
    }
    else {
        snprintf(out_path, out_path_size, BROWSERDLL_MOUNT_NAME ":/%s/%s", g_BrowserDllNroPath_1, nro_name);
    }
}

static Result _ossLoadDecompressBrowserDllNro(OssBrowserDllKind kind, OssBrowserDllNro nro, void **out_nro_buf, size_t *out_nro_size) {
    char nro_path[FS_MAX_PATH];
    _ossFormatBrowserDllNroPath(nro_path, sizeof(nro_path), kind, nro);
    extLogf("[oss] Loading NRO: '%s'", nro_path);

    FILE *f = fopen(nro_path, "rb");
    if(f == NULL) {
        return MAKERESULT(Module_Cruiser, CruiserResult_InvalidBrowserDllNro);
    }
    fseek(f, 0, SEEK_END);
    size_t nro_size = ftell(f);
    rewind(f);
    void *nro_buf = __libnx_aligned_alloc(PAGE_ALIGN, nro_size);
    if(nro_buf == NULL) {
        fclose(f);
        return MAKERESULT(Module_Cruiser, CruiserResult_InvalidBrowserDllNro);
    }
    if(fread(nro_buf, nro_size, 1, f) != 1) {
        __libnx_free(nro_buf);
        fclose(f);
        return MAKERESULT(Module_Cruiser, CruiserResult_InvalidBrowserDllNro);
    }
    fclose(f);

    if(!_ossIsBrowserDllNroLz4Compressed()) {
        *out_nro_buf = nro_buf;
        *out_nro_size = nro_size;
        return R_SUCCESS;
    }

    // Decompressed size is in the first 4 bytes of the LZ4 buffer
    size_t nro_dec_size = *(u32*)nro_buf;
    void *nro_lz4_buf_base = (u8*)nro_buf + sizeof(u32);
    size_t nro_lz4_base_size = nro_size - sizeof(u32);
    
    // Align it to 0x1000, as RO services will require later in module loading
    void *nro_dec_buf = __libnx_aligned_alloc(PAGE_ALIGN, nro_dec_size);
    if(nro_dec_buf == NULL) {
        __libnx_free(nro_buf);
        return MAKERESULT(Module_Cruiser, CruiserResult_InvalidBrowserDllNro);
    }

    // Get the decompressed NRO
    int dec_res = LZ4_decompress_safe(nro_lz4_buf_base, nro_dec_buf, nro_lz4_base_size, nro_dec_size);
    if(dec_res < 0) {
        extLogf("[oss] LZ4 decompression failed with error code: %d", dec_res);
        __libnx_free(nro_dec_buf);
        __libnx_free(nro_buf);
        return MAKERESULT(Module_Cruiser, CruiserResult_Lz4DecompressionFailed);
    }
    *out_nro_buf = nro_dec_buf;
    *out_nro_size = dec_res;
    __libnx_free(nro_buf);
    return R_SUCCESS;
}

Result ossInitialize(OssBrowserDllKind kind) {
    R_TRY(browserdllMount());
    R_TRY(ldInitialize(10));
    symInitialize();

    for(u32 i = 0; i < OssBrowserDllNro_Count; i++) {
        void *nro_buf = NULL;
        size_t nro_size = 0;
        R_TRY(_ossLoadDecompressBrowserDllNro(kind, i, &nro_buf, &nro_size));

        // Load each NRO module
        Result rc = ldLoadModuleFromNro(nro_buf, nro_size, g_BrowserDllNroNames[i], false);
        if(R_FAILED(rc)) {
            __libnx_free(nro_buf);
            return rc;
        }
    }

    return R_SUCCESS;
}

void ossExit(void) {
    ldExit();
    browserdllUnmount();
}
