#include "ipcext/account-ext.h"
#include <string.h>

Result accountextDeleteUser(const AccountUid uid) {
    return serviceDispatchIn(accountGetServiceSession(), 203, uid);
}

Result accountextGetProfileEditor(const AccountUid uid, AccountExtProfileEditor *out_editor) {
    memset(out_editor, 0, sizeof(AccountExtProfileEditor));
    return serviceDispatchIn(accountGetServiceSession(), 205, uid,
        .out_num_objects = 1,
        .out_objects = &out_editor->srv,
    );
}

Result accountextGetBaasAccountAdministrator(const AccountUid uid, AccountExtAdministrator *out_admin) {
    memset(out_admin, 0, sizeof(AccountExtAdministrator));
    return serviceDispatchIn(accountGetServiceSession(), 250, uid,
        .out_num_objects = 1,
        .out_objects = &out_admin->srv,
    );
}

Result accountextProfileEditorStore(AccountExtProfileEditor *editor, const AccountProfileBase *base, const AccountUserData *user_data) {
    return serviceDispatchIn(&editor->srv, 100, *base,
        .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_In | SfBufferAttr_HipcPointer },
        .buffers = { { user_data, sizeof(user_data) } },
    );
}

Result accountextProfileEditorStoreWithImage(AccountExtProfileEditor *editor, const AccountProfileBase *base, const AccountUserData *user_data, const u8 *icon_buf, const size_t icon_size) {
    return serviceDispatchIn(&editor->srv, 101, *base,
        .buffer_attrs = {
            SfBufferAttr_FixedSize | SfBufferAttr_In | SfBufferAttr_HipcPointer,
            SfBufferAttr_HipcMapAlias | SfBufferAttr_In,
        },
        .buffers = {
            { user_data, sizeof(user_data) },
            { icon_buf, icon_size },
        },
    );
}

Result accountextAdministratorGetAccountId(AccountExtAdministrator *admin, u64 *out_account_id) {
    return serviceDispatchOut(&admin->srv, 1, *out_account_id);
}

Result accountextAdministratorGetNintendoAccountId(AccountExtAdministrator *admin, u64 *out_nintendo_account_id) {
    return serviceDispatchOut(&admin->srv, 120, *out_nintendo_account_id);
}

Result accountextAdministratorDeleteRegistrationInfoLocally(AccountExtAdministrator *admin) {
    return serviceDispatch(&admin->srv, 203);
}

Result accountextAdministratorIsLinkedWithNintendoAccount(AccountExtAdministrator *admin, bool *out_is) {
    return serviceDispatchOut(&admin->srv, 250, *out_is);
}
