#include "ld/ldro.h"
#include "guard.h"

#define NX_SERVICE_ASSUME_NON_DOMAIN
static Service g_RoService;

NX_GENERATE_SERVICE_GUARD(ldro);

NX_INLINE Result _ldroCmdInitialize(void) {
    u64 pid_placeholder = 0;
    return serviceDispatchIn(&g_RoService, 4, pid_placeholder,
        .in_send_pid = true,
        .in_num_handles = 1,
        .in_handles = { CUR_PROCESS_HANDLE },
    );
}

Result _ldroInitialize(void) {
    EXT_R_TRY(smGetService(&g_RoService, LDRO_SERVICE_NAME));
    EXT_R_TRY(_ldroCmdInitialize());
    return EXT_R_SUCCESS;
}

void _ldroCleanup(void) {
    serviceClose(&g_RoService);
}

Result ldroMapManualLoadModuleMemory(uintptr_t *out_address, uintptr_t nro_address, size_t nro_size, uintptr_t bss_address, size_t bss_size) {
    const struct {
        u64 pid_placeholder;
        uintptr_t nro_address;
        size_t nro_size;
        uintptr_t bss_address;
        size_t bss_size;
    } in = { 0, nro_address, nro_size, bss_address, bss_size };
    return serviceDispatchInOut(&g_RoService, 0, in, *out_address, .in_send_pid = true);
}

Result ldroUnmapManualLoadModuleMemory(uintptr_t nro_address) {
    const struct {
        u64 pid_placeholder;
        uintptr_t nro_address;
    } in = { 0, nro_address };
    return serviceDispatchIn(&g_RoService, 1, in, .in_send_pid = true);
}

Result ldroRegisterModuleInfo(uintptr_t nrr_address, size_t nrr_size) {
    const struct {
        u64 pid_placeholder;
        uintptr_t nrr_address;
        size_t nrr_size;
    } in = { 0, nrr_address, nrr_size };
    return serviceDispatchIn(&g_RoService, 2, in, .in_send_pid = true);
}

Result ldroUnregisterModuleInfo(uintptr_t nrr_address) {
    const struct {
        u64 pid_placeholder;
        uintptr_t nrr_address;
    } in = { 0, nrr_address };
    return serviceDispatchIn(&g_RoService, 3, in, .in_send_pid = true);
}
