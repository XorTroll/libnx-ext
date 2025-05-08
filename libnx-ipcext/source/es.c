#include "ipcext/es.h"
#include "guard.h"

static Service g_EsService;

NX_GENERATE_SERVICE_GUARD(es);

Result _esInitialize(void) {
    return smGetService(&g_EsService, "es");
}

void _esCleanup(void) {
    serviceClose(&g_EsService);
}

Result esImportTicket(const void *tik, size_t tik_size, const void *cert, size_t cert_size) {
    return serviceDispatch(&g_EsService, 1,
        .buffer_attrs = {
            SfBufferAttr_HipcMapAlias | SfBufferAttr_In,
            SfBufferAttr_HipcMapAlias | SfBufferAttr_In,
        },
        .buffers = {
            { tik, tik_size },
            { cert, cert_size },
        },
    );
}

Result esDeleteTicket(const EsRightsId *rights_id) {
    return serviceDispatch(&g_EsService, 3,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
        .buffers = { { rights_id, sizeof(EsRightsId) }, },
    );
}

u32 esCountCommonTicket() {
    u32 count = 0;
    serviceDispatchOut(&g_EsService, 9, count);
    return count;
}

u32 esCountPersonalizedTicket() {
    u32 count = 0;
    serviceDispatchOut(&g_EsService, 10, count);
    return count;
}

Result esListCommonTicket(u32 *out_written, EsRightsId *out_rights_id_buf, size_t out_rights_id_buf_size) {
    return serviceDispatchOut(&g_EsService, 11, *out_written,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { { out_rights_id_buf, out_rights_id_buf_size } },
    );
}

Result esListPersonalizedTicket(u32 *out_written, EsRightsId *out_rights_id_buf, size_t out_rights_id_buf_size) {
    return serviceDispatchOut(&g_EsService, 12, *out_written,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { { out_rights_id_buf, out_rights_id_buf_size } },
    );
}
