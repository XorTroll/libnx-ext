
#pragma once
#include "ext.h"

typedef enum {
    OssBrowserDllKind_CfiDisabled,
    OssBrowserDllKind_CfiEnabled,

    OssBrowserDllKind_Count
} OssBrowserDllKind;

Result ossInitialize(OssBrowserDllKind kind);
void ossExit(void);
