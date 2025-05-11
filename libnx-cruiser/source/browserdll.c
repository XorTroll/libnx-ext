#include "cruiser/browserdll.h"

Result browserdllMount(void) {
    if(browserdllIsMounted()) {
        return EXT_R_SUCCESS;
    }

    EXT_R_TRY(romfsMountFromDataArchive(BROWSERDLL_DATA_ID, NcmStorageId_BuiltInSystem, BROWSERDLL_MOUNT_NAME));
    return EXT_R_SUCCESS;
}

bool browserdllIsMounted(void) {
    return fsdevGetDeviceFileSystem(BROWSERDLL_MOUNT_NAME) != NULL;
}

void browserdllUnmount(void) {
    if(!browserdllIsMounted()) {
        return;
    }

    romfsUnmount(BROWSERDLL_MOUNT_NAME);
}
