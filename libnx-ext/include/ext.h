// ...
#pragma once
#include <switch.h>

#define EXT_R_SUCCESS 0

#define EXT_R_TRY(expr) { \
    Result rc = R_VALUE(expr); \
    if(R_FAILED(rc)) { \
        return rc; \
    } \
}

#define EXT_PAGE_ALIGN 0x1000

#ifdef __cplusplus
#define EXT_STATIC_ASSERT static_assert
#else
#define EXT_STATIC_ASSERT _Static_assert
#endif

extern void *__libnx_alloc(size_t size);
extern void *__libnx_aligned_alloc(size_t align, size_t size);
extern void __libnx_free(void *ptr);

typedef void(*ExtLogFn)(const char*, size_t);

void extSetLogFunction(ExtLogFn fn);
void extLogf(const char *fmt, ...);
