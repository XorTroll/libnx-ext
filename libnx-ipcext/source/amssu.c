#include "ipcext/amssu.h"
#include "guard.h"
#include <string.h>

static Service g_AmsSuService;
static TransferMemory g_AmsSuTransferMemory;

NX_GENERATE_SERVICE_GUARD(amssu);

Result _amssuInitialize(void) {
    return smGetService(&g_AmsSuService, "ams:su");
}

void _amssuCleanup(void) {
    serviceClose(&g_AmsSuService);
    tmemClose(&g_AmsSuTransferMemory);
}

#define _AMSSU_MAKE_IPC_PATH(path) \
    char ipc_path[FS_MAX_PATH] = {}; \
    strncpy(ipc_path, path, FS_MAX_PATH - 1); \
    ipc_path[FS_MAX_PATH - 1] = '\0';

Result amssuGetUpdateInformation(const char *path, AmsSuUpdateInformation *out_info) {
    _AMSSU_MAKE_IPC_PATH(path);
    return serviceDispatchOut(&g_AmsSuService, 0, *out_info,
        .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize },
        .buffers = { { ipc_path, FS_MAX_PATH } },
    );
}

Result amssuValidateUpdate(const char *path, AmsSuUpdateValidationInfo *out_info, Result *out_rc, Result *out_exfat_rc) {
    _AMSSU_MAKE_IPC_PATH(path);

    struct {
        Result rc;
        Result exfat_rc;
        AmsSuUpdateValidationInfo info;
    } out;

    const Result rc = serviceDispatchOut(&g_AmsSuService, 1, out,
        .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize },
        .buffers = { { ipc_path, FS_MAX_PATH } },
    );

    if(R_SUCCEEDED(rc)) {
        *out_rc = out.rc;
        *out_exfat_rc = out.exfat_rc;
        *out_info = out.info;
    }
    return rc;
}

Result amssuSetupUpdate(void *buf, size_t size, const char *path, bool exfat) {
    Result rc = 0;
    _AMSSU_MAKE_IPC_PATH(path);

    if(buf == NULL) {
        rc = tmemCreate(&g_AmsSuTransferMemory, size, Perm_None);
    }
    else {
        rc = tmemCreateFromMemory(&g_AmsSuTransferMemory, buf, size, Perm_None);
    }
    if(R_FAILED(rc)) {
        return rc;
    }

    const struct {
        u8 exfat;
        u64 size;
    } in = { exfat, g_AmsSuTransferMemory.size };

    rc = serviceDispatchIn(&g_AmsSuService, 2, in,
        .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize },
        .buffers = { { ipc_path, FS_MAX_PATH } },
        .in_num_handles = 1,
        .in_handles = { g_AmsSuTransferMemory.handle },
    );

    if(R_FAILED(rc)) {
        tmemClose(&g_AmsSuTransferMemory);
    }
    return rc;
}

Result amssuSetupUpdateWithVariation(void *buf, size_t size, const char *path, bool exfat, NcmExtFirmwareVariationId variation_id) {
    Result rc = 0;
    _AMSSU_MAKE_IPC_PATH(path);

    if(buf == NULL) {
        rc = tmemCreate(&g_AmsSuTransferMemory, size, Perm_None);
    }
    else {
        rc = tmemCreateFromMemory(&g_AmsSuTransferMemory, buf, size, Perm_None);
    }
    if(R_FAILED(rc)) {
        return rc;
    }

    const struct {
        u8 exfat;
        NcmExtFirmwareVariationId variation_id;
        u64 size;
    } in = { exfat, variation_id, g_AmsSuTransferMemory.size };

    rc = serviceDispatchIn(&g_AmsSuService, 3, in,
        .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize },
        .buffers = { { ipc_path, FS_MAX_PATH } },
        .in_num_handles = 1,
        .in_handles = { g_AmsSuTransferMemory.handle },
    );

    if(R_FAILED(rc)) {
        tmemClose(&g_AmsSuTransferMemory);
    }
    return rc;
}

Result amssuRequestPrepareUpdate(AsyncResult *async) {
    memset(async, 0, sizeof(AsyncResult));

    Handle event_h = INVALID_HANDLE;
    const Result rc = serviceDispatch(&g_AmsSuService, 4,
        .out_num_objects = 1,
        .out_objects = &async->s,
        .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
        .out_handles = &event_h,
    );

    if(R_SUCCEEDED(rc)) {
        eventLoadRemote(&async->event, event_h, false);
    }
    return rc;
}

Result amssuGetPrepareUpdateProgress(NsSystemUpdateProgress *out) {
    return serviceDispatchOut(&g_AmsSuService, 5, *out);
}

Result amssuHasPreparedUpdate(bool *out_has) {
    return serviceDispatchOut(&g_AmsSuService, 6, *out_has);
}

Result amssuApplyPreparedUpdate() {
    return serviceDispatch(&g_AmsSuService, 7);
}
