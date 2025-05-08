#include "cruiser/sym.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <reent.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>
// #include <setjmp.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/iosupport.h>
#include <sys/time.h>
#include <sys/lock.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/dirent.h>
#include <sys/socket.h>

// These are all stdc(++) symbols that WKC libraries will try to resolve when loaded
// Force-expose them here

// std functions that are not exposed in regular headers (or I was lazy to find in which one :P)

extern void __cxa_pure_virtual(void);
extern int __cxa_guard_acquire(void *guard);
extern void __cxa_guard_release(void *guard);
extern void _ZdlPv(void *ptr);
extern void *_Znwm(size_t size);
extern void _ZdaPv(void *arrptr);
extern char *strdup(const char *str);
extern int ftruncate(int fd, off_t length);

SYM_SYMBOL void *g_ForceExposeStdSymbols[] = {
    &fmod,
    &powf,
    &atan2f,
    &tanf,
    &logf,
    &nextafter,
    &pthread_self,
    &strdup,
    &fprintf,
    &fwrite,
    &fputs,
    &lroundf,
    &cosf,
    &sinf,
    &lround,
    &snprintf,
    &remainder,
    &llroundf,
    &sqrtf,
    &log10f,
    &exp,
    &pow,
    &sin,
    &cos,
    &sqrt,
    &hypot,
    &logb,
    &atan2,
    &acos,
    &expf,
    &asinf,
    &log10,
    &tan,
    &acosf,
    &nextafterf,
    &lrint,
    &qsort,
    // &setjmp,
    // &longjmp,
    &__cxa_pure_virtual,
    &__cxa_guard_acquire,
    &__cxa_guard_release,
    &_ZdlPv,
    &_Znwm,
    &_ZdaPv,
    &fmodf,
    &strstr,
    &printf,
    &asin,
    &acosh,
    &asinh,
    &atanh,
    &cbrt,
    &cosh,
    &log2,
    &sinh,
    &tanh,
    &rand,
    &fgets,
    &memcpy,
    &memmove,
    &strlen,
    &memset,
    &sched_yield,
    &memcmp,
    &free,
    &fputc,
    &strncpy,
    &strcmp,
    &strtol,
    &memchr,
    &strcasecmp,
    &scalbn,
    &fflush,
    &bsearch,
    &expm1,
    &log1p,
    &sscanf,
    &getenv,
    &strchr,
    &fopen,
    &fclose,
    &strcpy,
    &strncasecmp,
    &strcspn,
    &strncat,
    &srand,
    &setvbuf,
    &vfprintf,
    &vsnprintf,
    &strrchr,
    &strcat,
    &exp2,
    &exp2f,
    &log2f,
    &gettimeofday,
    &access,
    &read,
    &close,
    &fstat,
    &ftruncate,
    &write,
    &mkdir,
    &strspn,
    &strpbrk,
    &tolower,
    &open,
    &strerror,
    &isxdigit,
    &atoi,
    &perror,
    &isalnum,
    &modf,
    &atof,
    &ferror,
    &getpid,
    &sleep,
    &lseek,
    &toupper,
    &clearerr
};
