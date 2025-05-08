#include "ipcext/bpcams.h"
#include "guard.h"

static Service g_BpcAmsService;

NX_GENERATE_SERVICE_GUARD(bpcams);

Result _bpcamsInitialize(void) {
    Handle tmp_port_h;
    const Result rc = svcConnectToNamedPort(&tmp_port_h, "bpc:ams");
    if(R_SUCCEEDED(rc)) {
        serviceCreate(&g_BpcAmsService, tmp_port_h);
    }
    return rc;
}

void _bpcamsCleanup(void) {
    serviceClose(&g_BpcAmsService);
}

Result bpcamsSetRebootPayload(void *payload_buf, size_t payload_size) {
    return serviceDispatch(&g_BpcAmsService, 65001,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
        .buffers = { { payload_buf, payload_size } },
    );
}
