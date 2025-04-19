
#pragma once
#include "ext.h"

#ifdef _CRUISER_API_IN_SOURCE
#define CRUISER_API(def) def
#else
#define CRUISER_API(def) extern def
#endif

Result cruiserInitialize(void);
void cruiserExit(void);
