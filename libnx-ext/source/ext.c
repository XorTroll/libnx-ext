#include "ext.h"
#include <stdarg.h>
#include <stdio.h>

static ExtLogFn g_LogFunction = NULL;
static Mutex g_LogMutex = {0};

void extSetLogFunction(ExtLogFn fn) {
    g_LogFunction = fn;
}

void extLogf(const char *fmt, ...) {
    mutexLock(&g_LogMutex);

    if(g_LogFunction == NULL) {
        mutexUnlock(&g_LogMutex);
        return;
    }

    va_list args;
    va_start(args, fmt);
    size_t len = vsnprintf(NULL, 0, fmt, args);
    char *buf = (char*)__libnx_alloc(len + 1);
    if(buf == NULL) {
        va_end(args);
        mutexUnlock(&g_LogMutex);
        return;
    }
    vsnprintf(buf, len + 1, fmt, args);
    g_LogFunction(buf, len);
    __libnx_free(buf);
    va_end(args);
    mutexUnlock(&g_LogMutex);
}
