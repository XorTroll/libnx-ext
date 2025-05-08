
#pragma once
#include "ext.h"

typedef enum {
    NsExtApplicationEvent_Launched = 0,
    NsExtApplicationEvent_LocalInstalled = 1,
    NsExtApplicationEvent_DownloadStarted = 2,
    NsExtApplicationEvent_Present = 3, // Installed or gamecard inserted
    NsExtApplicationEvent_Touched = 4,
    NsExtApplicationEvent_GamecardMissing = 5,
    NsExtApplicationEvent_Downloaded = 6,
    NsExtApplicationEvent_Unk7 = 7,
    NsExtApplicationEvent_Unk8 = 8,
    NsExtApplicationEvent_Unk9 = 9,
    NsExtApplicationEvent_Updated = 10,
    NsExtApplicationEvent_Archived = 11,
    NsExtApplicationEvent_UpdateNeeded = 12, // ???
    NsExtApplicationEvent_Unk13 = 13,
    NsExtApplicationEvent_Unk14 = 14,
    NsExtApplicationEvent_Unk15 = 15,
    NsExtApplicationEvent_AlreadyStarted = 16
} NsExtApplicationEvent;

typedef struct {
    NcmContentMetaKey meta_key;
    u64 storage_id;
} NsExtContentStorageMetaKey;

typedef struct {
    u64 id;
    u8 last_event;
    u8 attrs;
    u8 reserved[6];
    u64 last_updated;
} NsExtApplicationRecord;
EXT_STATIC_ASSERT(sizeof(NsExtApplicationRecord) == 0x18);

// All view flags I found to be used by qlaunch (names are pure guesses)

typedef enum {
    NsExtApplicationViewFlag_IsValid = BIT(0), // So far I have never seen this flag unset
    NsExtApplicationViewFlag_HasMainContents = BIT(1), // Games might have updates/DLC but not the main content
    NsExtApplicationViewFlag_Bit2 = BIT(2),
    NsExtApplicationViewFlag_Bit3 = BIT(3),
    NsExtApplicationViewFlag_HasContentsInstalled = BIT(4), // This flag is set for all installed applications with contents
    NsExtApplicationViewFlag_IsDownloading = BIT(5), // qlaunch calls GetApplicationViewDownloadErrorContext somewhere if this flag is set
    NsExtApplicationViewFlag_IsGameCard = BIT(6), // qlaunch calls EnsureGameCardAccess somewhere if this flag is set
    NsExtApplicationViewFlag_IsGameCardInserted = BIT(7), // qlaunch checks this flag along with the one above in many places, but this one is only set on gamecard games that are inserted
    NsExtApplicationViewFlag_CanLaunch = BIT(8), // qlaunch calls CheckApplicationLaunchVersion somewhere if this flag is set (and pctl ConfirmLaunchApplicationPermission somewhere else)
    NsExtApplicationViewFlag_NeedsUpdate = BIT(9), // qlaunch calls RequestApplicationUpdate somewhere if this flag is set, if IsValid is also set and if IsDownloading is not set
    NsExtApplicationViewFlag_CanLaunch2 = BIT(10), // Games seem to have set it along with CanLaunch
    NsExtApplicationViewFlag_Bit11 = BIT(11),
    NsExtApplicationViewFlag_Bit12 = BIT(12),
    NsExtApplicationViewFlag_NeedsVerify = BIT(13), // Games which need to be verified by qlaunch (due to corrupted data/etc) have this flag set
    NsExtApplicationViewFlag_IsWaitingCommit1 = BIT(14), // I guess, from "OptCntUpdate_DlgBodyWaitCommit" texts used if these flags are all set
    NsExtApplicationViewFlag_IsWaitingCommit2 = BIT(15),
    NsExtApplicationViewFlag_Bit16 = BIT(16),
    NsExtApplicationViewFlag_IsApplyingDelta = BIT(17), // I guess, from "IsApplyingDelta" text used if this flag is set
    NsExtApplicationViewFlag_IsWaitingCommit3 = BIT(18),
    NsExtApplicationViewFlag_Bit19 = BIT(19),
    NsExtApplicationViewFlag_Bit20 = BIT(20),
    NsExtApplicationViewFlag_Bit23 = BIT(23),
    NsExtApplicationViewFlag_Bit21 = BIT(21),
    NsExtApplicationViewFlag_Bit22 = BIT(22), // PromotionInfo related?
} NsExtApplicationViewFlag;

typedef struct {
    u64 app_id;
    u32 used32_x8;
    u32 flags;
    u64 download_progress_current;
    u64 download_progress_total;
    u32 used32_x20;
    u8 used8_x24_download_rel;
    u8 download_has_eta;
    u8 used8_x26;
    u8 unk_x27;
    u64 used64_x28;
    u64 other_progress_current;
    u64 other_progress_total;
    u32 used32_x40;
    u8 used8_x44;
    u8 used8_x45;
    u8 unk_x46[2];
    u64 used64_x48;
} NsExtApplicationView;
EXT_STATIC_ASSERT(sizeof(NsExtApplicationView) == 0x50);

NX_CONSTEXPR bool nsextApplicationViewHasFlags(const NsExtApplicationView *view, const u32 flags) {
    return (view->flags & flags) != 0;
}

typedef struct {
    u8 storage_id;
    u8 reserved[7];
    size_t app_size;
    size_t patch_size;
    size_t add_on_content_size;
} NsExtApplicationOccupiedSizeEntity;
EXT_STATIC_ASSERT(sizeof(NsExtApplicationOccupiedSizeEntity) == 0x20);

typedef struct {
    NsExtApplicationOccupiedSizeEntity entities[4];
} NsExtApplicationOccupiedSize;
EXT_STATIC_ASSERT(sizeof(NsExtApplicationOccupiedSize) == 0x80);

Result nsextPushApplicationRecord(const u64 app_id, const NsExtApplicationEvent last_event, const NsExtContentStorageMetaKey *cnt_storage_record_buf, const size_t cnt_storage_record_buf_count);
Result nsextListApplicationRecordContentMeta(const u64 offset, const u64 app_id, NsExtContentStorageMetaKey *out_buf, const size_t out_buf_count, u32 *out_count);
Result nsextDeleteApplicationRecord(const u64 app_id);
Result nsextPushLaunchVersion(const u64 app_id, const u32 launch_version);
