
#pragma once
#include "ext.h"

typedef struct {
    Service srv;
} AccountExtProfileEditor;

NX_INLINE void accountextProfileEditorClose(AccountExtProfileEditor *editor) {
    serviceClose(&editor->srv);
}

typedef struct {
    Service srv;
} AccountExtAdministrator;

NX_INLINE void accountextAdministratorClose(AccountExtAdministrator *admin) {
    serviceClose(&admin->srv);
}

Result accountextDeleteUser(const AccountUid uid);
Result accountextGetProfileEditor(const AccountUid uid, AccountExtProfileEditor *out_editor);
Result accountextGetBaasAccountAdministrator(const AccountUid uid, AccountExtAdministrator *out_admin);

Result accountextProfileEditorStore(AccountExtProfileEditor *editor, const AccountProfileBase *base, const AccountUserData *user_data);
Result accountextProfileEditorStoreWithImage(AccountExtProfileEditor *editor, const AccountProfileBase *base, const AccountUserData *user_data, const u8 *icon_buf, const size_t icon_size);

Result accountextAdministratorGetAccountId(AccountExtAdministrator *admin, u64 *out_account_id);
Result accountextAdministratorGetNintendoAccountId(AccountExtAdministrator *admin, u64 *out_nintendo_account_id);
Result accountextAdministratorDeleteRegistrationInfoLocally(AccountExtAdministrator *admin);
Result accountextAdministratorIsLinkedWithNintendoAccount(AccountExtAdministrator *admin, bool *out_is);

