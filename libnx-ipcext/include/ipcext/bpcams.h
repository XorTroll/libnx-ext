
#pragma once
#include "ext.h"

Result bpcamsInitialize(void);
void bpcamsExit(void);

Result bpcamsSetRebootPayload(void *payload_buf, size_t payload_size);
