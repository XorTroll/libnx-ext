#include "cruiser/browserdll.h"

Result browserdllMount(void) {
    if(browserdllIsMounted()) {
        return R_SUCCESS;
    }

    R_TRY(romfsMountFromDataArchive(BROWSERDLL_DATA_ID, NcmStorageId_BuiltInSystem, BROWSERDLL_MOUNT_NAME));
    return R_SUCCESS;
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
