#include "ipcext/ns-ext.h"

NX_INLINE Service *_nsextGetApplicationManagerInterfaceService(Service *srv_ref) {
    if(hosversionAtLeast(3,0,0)) {
        nsGetApplicationManagerInterface(srv_ref);
        return srv_ref;
    }
    else {
        return nsGetServiceSession_ApplicationManagerInterface();
    }
}

NX_INLINE void _nsextDisposeApplicationManagerInterfaceService(Service *srv_ref) {
    if(hosversionAtLeast(3,0,0)) {
        serviceClose(srv_ref);
    }
}

Result nsextPushApplicationRecord(const u64 app_id, const NsExtApplicationEvent last_event, const NsExtContentStorageMetaKey *cnt_storage_record_buf, const size_t cnt_storage_record_buf_count) {
    Service srv;
    Service *use_srv = _nsextGetApplicationManagerInterfaceService(&srv);

    const struct {
        u8 last_event;
        u8 pad[7];
        u64 app_id;
    } in = { last_event, {}, app_id };
    
    const Result rc = serviceDispatchIn(use_srv, 16, in,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
        .buffers = { { cnt_storage_record_buf, cnt_storage_record_buf_count * sizeof(NsExtContentStorageMetaKey) } },
    );
    _nsextDisposeApplicationManagerInterfaceService(&srv);
    return rc;
}

Result nsextListApplicationRecordContentMeta(const u64 offset, const u64 app_id, NsExtContentStorageMetaKey *out_buf, const size_t out_buf_count, u32 *out_count) {
    Service srv;
    Service *use_srv = _nsextGetApplicationManagerInterfaceService(&srv);

    const struct {
        u64 offset;
        u64 app_id;
    } in = { offset, app_id };

    const Result rc = serviceDispatchInOut(use_srv, 17, in, *out_count,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { { out_buf, out_buf_count * sizeof(NsExtContentStorageMetaKey) } },
    );
    _nsextDisposeApplicationManagerInterfaceService(&srv);
    return rc;
}

Result nsextDeleteApplicationRecord(const u64 app_id) {
    Service srv;
    Service *use_srv = _nsextGetApplicationManagerInterfaceService(&srv);

    const Result rc = serviceDispatchIn(use_srv, 27, app_id);
    _nsextDisposeApplicationManagerInterfaceService(&srv);
    return rc;
}

Result nsextPushLaunchVersion(const u64 app_id, const u32 launch_version) {
    Service srv;
    Service *use_srv = _nsextGetApplicationManagerInterfaceService(&srv);

    const struct {
        u32 version;
        u64 app_id;
        u8 pad[4];
    } in = { launch_version, app_id, {} };

    const Result rc = serviceDispatchIn(use_srv, 36, in);
    _nsextDisposeApplicationManagerInterfaceService(&srv);
    return rc;
}
