
#pragma once
#include "ext.h"

#define LDRO_SERVICE_NAME "ldr:ro"

Result ldroInitialize(void);
void ldroExit(void);

Result ldroMapManualLoadModuleMemory(uintptr_t *out_address, uintptr_t nro_address, size_t nro_size, uintptr_t bss_address, size_t bss_size);
Result ldroUnmapManualLoadModuleMemory(uintptr_t nro_address);

Result ldroRegisterModuleInfo(uintptr_t nrr_address, size_t nrr_size);
Result ldroUnregisterModuleInfo(uintptr_t nrr_address);
