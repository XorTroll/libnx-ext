
#pragma once
#include "ext.h"

typedef u32 NcmExtFirmwareVariationId;

typedef struct {
    u64 id;
    u32 version;
    u8 type; // NcmContentMetaType
    u8 platform; // NcmContentMetaPlatform
    u16 extended_header_size;
    u16 content_count;
    u16 content_meta_count;
    u8 attributes;
    u8 storage_id;
    u8 install_type; // NcmContentInstallType
    bool committed;
    u32 required_download_system_version;
    u8 reserved[4];
} NcmExtPackagedContentMetaHeader;
EXT_STATIC_ASSERT(sizeof(NcmExtPackagedContentMetaHeader) == 0x20);

typedef struct {
    u64 app_id;
    u32 extended_data_size;
    u8 pad[4];
} NcmExtDeltaMetaExtendedHeader;

Result ncmextReadApplicationControlDataManual(SetLanguage lang, u64 application_id, NsApplicationControlData *out_control_data, size_t size, size_t *actual_size, SetLanguage *out_lang);
