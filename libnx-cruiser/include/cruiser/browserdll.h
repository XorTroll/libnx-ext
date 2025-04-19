
#pragma once
#include "ext.h"

#define BROWSERDLL_MOUNT_NAME "browserdll"
#define BROWSERDLL_DATA_ID 0x0100000000000803

Result browserdllMount(void);
bool browserdllIsMounted(void);
void browserdllUnmount(void);
