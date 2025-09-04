#include "ipcext/ncm-ext.h"
#include "ipcext/result.h"
#include <string.h>

static Result _ncmextGetApplicationControlContentPath(NcmStorageId id, u64 app_or_patch_id, char *out_path) {
    NcmContentMetaDatabase meta_db;
    EXT_R_TRY(ncmOpenContentMetaDatabase(&meta_db, id));
    NcmContentStorage storage;
    EXT_R_TRY(ncmOpenContentStorage(&storage, id));

    NcmContentMetaKey key;
    Result rc = ncmContentMetaDatabaseGetLatestContentMetaKey(&meta_db, &key, app_or_patch_id);
    if(R_FAILED(rc)) {
        ncmContentMetaDatabaseClose(&meta_db);
        ncmContentStorageClose(&storage);
        return rc;
    }

    NcmContentId control_id;
    rc = ncmContentMetaDatabaseGetContentIdByType(&meta_db, &control_id, &key, NcmContentType_Control);
    if(R_FAILED(rc)) {
        ncmContentMetaDatabaseClose(&meta_db);
        ncmContentStorageClose(&storage);
        return rc;
    }

    rc = ncmContentStorageGetPath(&storage, out_path, FS_MAX_PATH, &control_id);
    if(R_FAILED(rc)) {
        ncmContentMetaDatabaseClose(&meta_db);
        ncmContentStorageClose(&storage);
        return rc;
    }

    ncmContentMetaDatabaseClose(&meta_db);
    ncmContentStorageClose(&storage);
    return 0;
}

Result _ncmextReadFile(FsFileSystem *fs, const char *file_path, void *out_buf, const size_t buf_size, size_t *out_size) {
    // libnx does not internally create a proper sized path buffer in all fs commands (like in this one), so we need to do it manually
    char fsp_path[FS_MAX_PATH] = {};
    strncpy(fsp_path, file_path, FS_MAX_PATH - 1);

    FsFile file;
    EXT_R_TRY(fsFsOpenFile(fs, fsp_path, FsOpenMode_Read, &file));

    s64 file_size;
    Result rc = fsFileGetSize(&file, &file_size);
    if(R_FAILED(rc)) {
        fsFileClose(&file);
        return rc;
    }

    if(file_size > buf_size) {
        fsFileClose(&file);
        return MAKERESULT(Module_Libnx, LibnxError_BadInput);
    }

    *out_size = 0;
    rc = fsFileRead(&file, 0, out_buf, buf_size, FsReadOption_None, out_size);
    if(R_FAILED(rc)) {
        fsFileClose(&file);
        return rc;
    }

    fsFileClose(&file);
    return 0;
}

NX_CONSTEXPR const char *_ncmextGetControlIconPath(SetLanguage lang) {
    switch(lang) {
        case SetLanguage_JA: return "/icon_Japanese.dat";
        case SetLanguage_FR: return "/icon_French.dat";
        case SetLanguage_DE: return "/icon_German.dat";
        case SetLanguage_IT: return "/icon_Italian.dat";
        case SetLanguage_ES: return "/icon_Spanish.dat";
        case SetLanguage_ZHCN: return "/icon_Chinese.dat";
        case SetLanguage_KO: return "/icon_Korean.dat";
        case SetLanguage_NL: return "/icon_Dutch.dat";
        case SetLanguage_PT: return "/icon_Portuguese.dat";
        case SetLanguage_RU: return "/icon_Russian.dat";
        case SetLanguage_ZHTW: return "/icon_Taiwanese.dat";
        case SetLanguage_ENGB: return "/icon_BritishEnglish.dat";
        case SetLanguage_FRCA: return "/icon_CanadianFrench.dat";
        case SetLanguage_ES419: return "/icon_LatinAmericanSpanish.dat";
        case SetLanguage_ZHHANS: return "/icon_ChineseSimplified.dat";
        case SetLanguage_ZHHANT: return "/icon_ChineseTraditional.dat";
        case SetLanguage_PTBR: return "/icon_BrazilianPortuguese.dat";

        case SetLanguage_ENUS: default: return "/icon_AmericanEnglish.dat";
    }
}

Result ncmextReadApplicationControlDataManual(SetLanguage lang, u64 application_id, NsApplicationControlData *out_control_data, size_t size, size_t *actual_size, SetLanguage *out_lang) {
    if(size < sizeof(NsApplicationControlData)) {
        return MAKERESULT(Module_Libnx, LibnxError_BadInput);
    }

    u64 patch_id = application_id ^ 0x800;
    char control_path[FS_MAX_PATH];
    if(R_FAILED(_ncmextGetApplicationControlContentPath(NcmStorageId_SdCard, patch_id, control_path))
    && R_FAILED(_ncmextGetApplicationControlContentPath(NcmStorageId_BuiltInUser, patch_id, control_path))
    && R_FAILED(_ncmextGetApplicationControlContentPath(NcmStorageId_GameCard, patch_id, control_path))
    && R_FAILED(_ncmextGetApplicationControlContentPath(NcmStorageId_SdCard, application_id, control_path))
    && R_FAILED(_ncmextGetApplicationControlContentPath(NcmStorageId_BuiltInUser, application_id, control_path))
    && R_FAILED(_ncmextGetApplicationControlContentPath(NcmStorageId_GameCard, application_id, control_path))) {
        return MAKERESULT(Module_LibnxIpcExt, LibnxIpcExtError_ControlNotFound);
    }

    FsFileSystem control_fs;
    EXT_R_TRY(fsOpenFileSystemWithId(&control_fs, application_id, FsFileSystemType_ContentControl, control_path, FsContentAttributes_All));

    size_t nacp_size;
    Result rc = _ncmextReadFile(&control_fs, "/control.nacp", &out_control_data->nacp, sizeof(out_control_data->nacp), &nacp_size);
    if(R_FAILED(rc)) {
        fsFsClose(&control_fs);
        return rc;
    }
    if(nacp_size != sizeof(out_control_data->nacp)) {
        fsFsClose(&control_fs);
        return MAKERESULT(Module_Libnx, LibnxError_BadInput);
    }

    size_t icon_size;
    rc = _ncmextReadFile(&control_fs, _ncmextGetControlIconPath(lang), out_control_data->icon, sizeof(out_control_data->icon), &icon_size);
    if(R_FAILED(rc)) {
        // The icon might not be present, so try every single language until we find one that exists
        bool found_icon = false;
        for(SetLanguage l = SetLanguage_JA; l < SetLanguage_Total; l++) {
            rc = _ncmextReadFile(&control_fs, _ncmextGetControlIconPath(l), out_control_data->icon, sizeof(out_control_data->icon), &icon_size);
            if(R_SUCCEEDED(rc)) {
                *out_lang = l;
                found_icon = true;
                break;
            }
        }

        if(!found_icon) {
            fsFsClose(&control_fs);
            return rc;
        }
    }
    else {
        *out_lang = lang;
    }

    *actual_size = nacp_size + icon_size;
    fsFsClose(&control_fs);
    return 0;
}
