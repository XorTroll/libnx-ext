
#pragma once
#include "ext.h"
#include "ipcext/ncm-ext.h"

#define IPCEXT_AMSSU_FW_VARIATION_COUNT_MAX 16

typedef struct {
    u32 version;
    bool exfat_supported;
    u32 fw_variation_count;
    NcmExtFirmwareVariationId fw_variation_ids[IPCEXT_AMSSU_FW_VARIATION_COUNT_MAX];
} AmsSuUpdateInformation;

typedef struct {
    NcmContentMetaKey invalid_key;
    NcmContentId invalid_content_id;
} AmsSuUpdateValidationInfo;

typedef struct {
    s64 current_size;
    s64 total_size;
} AmsSuSystemUpdateProgress;

Result amssuInitialize();
void amssuExit();

Result amssuGetUpdateInformation(const char *path, AmsSuUpdateInformation *out_info);
Result amssuValidateUpdate(const char *path, AmsSuUpdateValidationInfo *out_info, Result *out_rc, Result *out_exfat_rc);
Result amssuSetupUpdate(void *buf, size_t size, const char *path, bool exfat);
Result amssuSetupUpdateWithVariation(void *buf, size_t size, const char *path, bool exfat, NcmExtFirmwareVariationId variation_id);
Result amssuRequestPrepareUpdate(AsyncResult *async);
Result amssuGetPrepareUpdateProgress(NsSystemUpdateProgress *out);
Result amssuHasPreparedUpdate(bool *out_has);
Result amssuApplyPreparedUpdate();
