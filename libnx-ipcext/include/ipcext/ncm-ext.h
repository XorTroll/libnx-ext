
#pragma once
#include "ext.h"

typedef u32 NcmExtFirmwareVariationId;

typedef struct {
    u64 app_id;
    u32 version;
    u8 type;
    u8 pad;
    NcmContentMetaHeader header;
    u8 pad1[0x2];
    u32 required_download_system_version;
    u8 pad2[0x4];
} NcmExtPackagedContentMetaHeader;

typedef struct {
    u64 app_id;
    u32 extended_data_size;
    u8 reserved[4];
} NcmExtDeltaMetaExtendedHeader;
