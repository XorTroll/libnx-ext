// ...
#pragma once
#include <switch.h>

#define R_SUCCESS 0

#define R_TRY(expr) { \
    Result rc = R_VALUE(expr); \
    if(R_FAILED(rc)) { \
        return rc; \
    } \
}

#define PAGE_ALIGN 0x1000

extern void *__libnx_alloc(size_t size);
extern void *__libnx_aligned_alloc(size_t align, size_t size);
extern void __libnx_free(void *ptr);

typedef void(*ExtLogFn)(const char*, size_t);

void extSetLogFunction(ExtLogFn fn);
void extLogf(const char *fmt, ...);
