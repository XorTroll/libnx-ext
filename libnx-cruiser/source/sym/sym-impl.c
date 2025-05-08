#include "cruiser/sym.h"
#include "cruiser/result.h"
#include "ext.h"

// We cannot include most of the fundamental C headers here since we would have conflicts with below already defined macros/etc that we need to have as symbols
// The prime examples of this are stdout/stderr/stdin, which our newlib exposes as macros but nnSdk/musl exposes as symbols

#include <stdlib.h>
#include <reent.h>
#include <errno.h>

extern void *memset(void *ptr, int value, size_t num);
extern char *strncpy(char *dest, const char *src, size_t num);
extern int snprintf(char *str, size_t size, const char *format, ...);

typedef struct {
    Thread libnx_thread; u8 remaining[0x40 - sizeof(Thread)]; // u8 AllThreadsListNode_MultiWaitObjectList_Reserved[0x40];
    u8 State;
    u8 StackIsAliased;
    u8 AutoRegistered;
    u8 SuspendCount;
    u16 BasePriority;
    u16 Version;
    void *OriginalStack;
    void *Stack;
    size_t StackSize;
    void *Argument;
    void *ThreadFunction;
    void *CurrentFiber;
    void *InitialFiber;
    u8 TlsValueArray[0x100];
    char ThreadNameBuffer[0x20];
    char *ThreadNamePointer;
    u32 CsThread;
    u32 CvTHread;
    Handle handle;
    u8 LockHistory;
    u64 ThreadId;
} NnOsThreadType;
static_assert(sizeof(NnOsThreadType) == 0x1C0);

#define MAX_THREAD_COUNT 10
NnOsThreadType *g_ThreadList[MAX_THREAD_COUNT];
u32 g_ThreadListCount = 0;

void AllocateThread(NnOsThreadType *thread) {
    if(g_ThreadListCount >= MAX_THREAD_COUNT) {
        extLogf("Thread list is full!");
        diagAbortWithResult(MAKERESULT(Module_Cruiser, CruiserResult_CalledStubbedFakeSymbol));
    }
    g_ThreadList[g_ThreadListCount++] = thread;

    thread->handle = thread->libnx_thread.handle;
    thread->Argument = (void*)0xEEFF0011;
    thread->ThreadFunction = (void*)0xAABBCCDD;
    svcGetThreadId(&thread->ThreadId, thread->libnx_thread.handle);
    snprintf(thread->ThreadNameBuffer, sizeof(thread->ThreadNameBuffer), "FakeThread_%ld", thread->ThreadId);
    thread->ThreadNamePointer = thread->ThreadNameBuffer;

    s32 prio;
    svcGetThreadPriority(&prio, thread->libnx_thread.handle);
    thread->BasePriority = prio - 28;

    thread->State = 0;
    thread->StackIsAliased = 0;
    thread->AutoRegistered = 0;
    thread->SuspendCount = 0;
    thread->Version = 0;
    thread->OriginalStack = thread->libnx_thread.stack_mem;
    thread->Stack = thread->libnx_thread.stack_mirror;
    thread->StackSize = thread->libnx_thread.stack_sz;
}

NX_INLINE void SetAllocatedThreadName(NnOsThreadType *thread, const char *name) {
    strncpy(thread->ThreadNameBuffer, name, sizeof(thread->ThreadNameBuffer) - 1);
}

NX_INLINE NnOsThreadType *FindAllocatedThreadByHandle(Handle handle) {
    for(u32 i = 0; i < g_ThreadListCount; i++) {
        if(g_ThreadList[i]->handle == handle) {
            return g_ThreadList[i];
        }
    }
    
    extLogf("Thread with handle %d not found!", handle);
    diagAbortWithResult(MAKERESULT(Module_Cruiser, CruiserResult_CalledStubbedFakeSymbol));
}

NX_INLINE NnOsThreadType *GetCurrentAllocatedThread() {
    return FindAllocatedThreadByHandle(threadGetCurHandle());
}

#define _SYM_LOGF(fmt, ...) ({ \
    extLogf("[hwkc-fakesym] (%s) " fmt, GetCurrentAllocatedThread()->ThreadNamePointer, ##__VA_ARGS__); \
})

#define _SYM_ABORTF(fmt, ...) ({ \
    _SYM_LOGF(fmt, ##__VA_ARGS__); \
    diagAbortWithResult(MAKERESULT(Module_Cruiser, CruiserResult_CalledStubbedFakeSymbol)); \
})

///////////////////////////////////////////////////////////////////////////////////////////

// Symbol wrappers: most of these are just stubs that abort when called for now
// (C/C++ standard library functions present in both newlib and nnSdk/musl are in sym-std.c)
// These are the ones that are not present in newlib but are in nnSdk/musl, and we need to fake/wrap them

SYM_SYMBOL void *stdout = NULL;
SYM_SYMBOL void *stderr = NULL;
SYM_SYMBOL void *stdin = NULL;

extern int custom_setjmp(void *buf);
extern void custom_longjmp(void *buf, int val);

SYM_SYMBOL int setjmp(void *buf) {
    _SYM_LOGF("setjmp called! buf=%p", buf);
    return custom_setjmp(buf);
}

SYM_SYMBOL void longjmp(void *buf, int val) {
    _SYM_LOGF("longjmp called! buf=%p, val=%d", buf, val);
    custom_longjmp(buf, val);
}

SYM_SYMBOL void _ZNSt3__16__sortIRNS_6__lessIjjEEPjEEvT0_S5_T_(void *a, void *b) {
    _SYM_ABORTF("std::__sort<std::__less<uint,uint> &,uint *>(uint *,uint *,std::__less<uint,uint> &) called!");
}

SYM_SYMBOL void _ZNSt3__122__libcpp_verbose_abortEPKcz(const char *fmt, ...) {
    _SYM_ABORTF("std::__libcpp_verbose_abort(char const*, ...) called!");
}

SYM_SYMBOL void _ZNSt3__16__sortIRNS_6__lessIiiEEPiEEvT0_S5_T_(void *a, void *b) {
    _SYM_ABORTF("std::__sort<std::__less<int,int> &,int *>(int *,int *,std::__less<int,int> &) called!");
}

SYM_SYMBOL void _ZNSt3__112__next_primeEm(unsigned long a) {
    _SYM_ABORTF("std::__next_prime(unsigned long) called!");
}

SYM_SYMBOL int *__errno_location(void) {
    return __errno();
}

SYM_SYMBOL void *iconv_open(const char *tocode, const char *fromcode) {
    _SYM_ABORTF("iconv_open(%s, %s) called!", tocode, fromcode);
}

SYM_SYMBOL size_t iconv(void *cd, const char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft) {
    _SYM_ABORTF("iconv(%p, %p, %p, %p, %p) called!", cd, inbuf, inbytesleft, outbuf, outbytesleft);
}

SYM_SYMBOL void iconv_close(void *cd) {
    _SYM_ABORTF("iconv_close(%p) called!", cd);
}

SYM_SYMBOL void __fpclassify(float x) {
    _SYM_ABORTF("__fpclassify(%f) called!", x);
}

// Our std implementation of strerror_r defines it as a macro, we need to expose it as an actual symbol (like stdout/etc above)
 
extern int __xpg_strerror_r(int errnum, char *buf, size_t buflen);

SYM_SYMBOL int strerror_r(int errnum, char *buf, size_t buflen) {
    return __xpg_strerror_r(errnum, buf, buflen);
}

///////////////////////// Symbols from nnSdk that we also must fake

SYM_SYMBOL int _nnmusl_socket_TranslateNnmuslFileDescriptorOrPassThrough(int fd) {
    _SYM_LOGF("_nnmusl_socket_TranslateNnmuslFileDescriptorOrPassThrough() called!");

    // (Note: In the implementation at OfflineWeb, they only handle a special case for fds where upper-16-bits=0x303)
    return fd;
}

// https://github.com/syvb/switch-oss/blob/master/src_2.26.0/rocrt_nro.cpp

SYM_SYMBOL int __nnmusl_init_dso(u8 *EX_start, u8 *EX_end,
        u8 *tdata_start, u8 *tdata_end,
        u8 *tdata_align_abs, u8 *tdata_align_rel,
        u8 *tbss_start, u8 *tbss_end,
        u8 *tbss_align_abs, u8 *tbss_align_rel,
        u8 *got_plt_start, u8 *got_plt_end,
        u8 *rela_dyn_start, u8 *rela_dyn_end,
        u8 *rel_dyn_start, u8 *rel_dyn_end,
        u8 *rela_plt_start, u8 *rela_plt_end,
        u8 *rel_plt_start, u8 *rel_plt_end,
        u8 *DYNAMIC) {
    _SYM_LOGF("__nnmusl_init_dso(EX_start=%p, EX_end=%p, tdata_start=%p, tdata_end=%p, tdata_align_abs=%p, tdata_align_rel=%p, tbss_start=%p, tbss_end=%p, tbss_align_abs=%p, tbss_align_rel=%p, got_plt_start=%p, got_plt_end=%p, rela_dyn_start=%p, rela_dyn_end=%p, rel_dyn_start=%p, rel_dyn_end=%p, rela_plt_start=%p, rela_plt_end=%p, rel_plt_start=%p, rel_plt_end=%p) called!", EX_start, EX_end, tdata_start, tdata_end, tdata_align_abs, tdata_align_rel, tbss_start, tbss_end, tbss_align_abs, tbss_align_rel, got_plt_start, got_plt_end, rela_dyn_start, rela_dyn_end, rel_dyn_start, rel_dyn_end, rela_plt_start, rela_plt_end, rel_plt_start, rel_plt_end);

    // Returning 0 makes the module itself call the init-array, while returning 1 skips init-array calls
    return 0;
}

SYM_SYMBOL void __nnmusl_fini_dso(u8 *EX_start, u8 *EX_end, u8 *tdata_start, u8 *tdata_end, u8 *tbss_start, u8 *tbss_end) {
    _SYM_LOGF("__nnmusl_fini_dso(EX_start=%p, EX_end=%p, tdata_start=%p, tdata_end=%p, tbss_start=%p, tbss_end=%p) called!", EX_start, EX_end, tdata_start, tdata_end, tbss_start, tbss_end);
}

SYM_SYMBOL void _ZN2nn2ro12ProtectRelroEPKvS2_S2_S2_S2_(const void* relro, const void* relroEnd, const void* fullRelroEnd, const void* pModule, const void* pVersion) {
    _SYM_LOGF("nn::ro::ProtectRelro(relro=%p, relroEnd=%p, fullRelroEnd=%p, pModule=%p, pVersion=%p) called!", relro, relroEnd, fullRelroEnd, pModule, pVersion);
    // TODO: implement this?
}

typedef struct {
    u8 data[0x20];
} NnOsMutexType;

typedef struct {
    u8 a;
    bool is_recursive;
    u8 b[0x6];
    RMutex rmutex; // mutex and rmutex are placed in fields that are initialized to 0 by inlined nn::os::InitializeMutex (same initialization as our (R)Mutex), and not hiding the "is recursive" field
    u8 c[0x4];
    Mutex mutex;
    u8 d[0x4];
} CustomMutexType;

_Static_assert(sizeof(NnOsMutexType) == 0x20);
_Static_assert(sizeof(CustomMutexType) == sizeof(NnOsMutexType));

SYM_SYMBOL void _ZN2nn2os15InitializeMutexEPNS0_9MutexTypeEbi(NnOsMutexType *mutex, bool is_recursive, int lock_level) {
    // _SYM_LOGF("nn::os::InitializeMutex(nn::os::MutexType *, bool, int) called!");

    CustomMutexType *our_mutex = (CustomMutexType*)mutex;
    our_mutex->is_recursive = is_recursive;
    if(our_mutex->is_recursive) {
        // _SYM_LOGF("Initializing recursive mutex %p (lock level %d)", mutex, lock_level);
        rmutexInit(&our_mutex->rmutex);
    }
    else {
        // _SYM_LOGF("Initializing mutex %p (lock level %d)", mutex, lock_level);
        mutexInit(&our_mutex->mutex);
    }
}

SYM_SYMBOL void _ZN2nn2os13FinalizeMutexEPNS0_9MutexTypeE(void) {
    _SYM_ABORTF("_ZN2nn2os13FinalizeMutexEPNS0_9MutexTypeE");
    // TODO
}

SYM_SYMBOL void _ZN2nn2fs8OpenFileEPNS0_10FileHandleEPKci(void) {
    _SYM_ABORTF("_ZN2nn2fs8OpenFileEPNS0_10FileHandleEPKci");
    // TODO
}

SYM_SYMBOL void _ZN2nn2os9LockMutexEPNS0_9MutexTypeE(NnOsMutexType *mutex) {
    // _SYM_LOGF("nn::os::LockMutex(nn::os::MutexType *) called!");

    CustomMutexType *our_mutex = (CustomMutexType*)mutex;
    if(our_mutex->is_recursive) {
        // _SYM_LOGF("Locking recursive mutex %p", mutex);
        rmutexLock(&our_mutex->rmutex);
    }
    else {
        // _SYM_LOGF("Locking mutex %p", mutex);
        mutexLock(&our_mutex->mutex);
    }
}

SYM_SYMBOL void _ZN2nn2os11UnlockMutexEPNS0_9MutexTypeE(NnOsMutexType *mutex) {
    // _SYM_LOGF("nn::os::UnlockMutex(nn::os::MutexType *) called!");

    CustomMutexType *our_mutex = (CustomMutexType*)mutex;
    if(our_mutex->is_recursive) {
        // _SYM_LOGF("Unlocking recursive mutex %p", mutex);
        rmutexUnlock(&our_mutex->rmutex);
    }
    else {
        // _SYM_LOGF("Unlocking mutex %p", mutex);
        mutexUnlock(&our_mutex->mutex);
    }
}

SYM_SYMBOL void _ZN2nn2fs9CloseFileENS0_10FileHandleE(void) {
    _SYM_ABORTF("_ZN2nn2fs9CloseFileENS0_10FileHandleE");
    // TODO
}

SYM_SYMBOL void _ZN2nn2fs8ReadFileEPmNS0_10FileHandleElPvm(void) {
    _SYM_ABORTF("_ZN2nn2fs8ReadFileEPmNS0_10FileHandleElPvm");
    // TODO
}

SYM_SYMBOL void _ZN2nn2fs10CreateFileEPKcl(void) {
    _SYM_ABORTF("_ZN2nn2fs10CreateFileEPKcl");
    // TODO
}

SYM_SYMBOL void _ZN2nn2fs11GetFileSizeEPlNS0_10FileHandleE(void) {
    _SYM_ABORTF("_ZN2nn2fs11GetFileSizeEPlNS0_10FileHandleE");
    // TODO
}

SYM_SYMBOL void _ZN2nn2fs9WriteFileENS0_10FileHandleElPKvmRKNS0_11WriteOptionE(void) {
    _SYM_ABORTF("_ZN2nn2fs9WriteFileENS0_10FileHandleElPKvmRKNS0_11WriteOptionE");
    // TODO
}

SYM_SYMBOL void _ZN2nn2fs9FlushFileENS0_10FileHandleE(void) {
    _SYM_ABORTF("_ZN2nn2fs9FlushFileENS0_10FileHandleE");
    // TODO
}

SYM_SYMBOL void _ZN2nn2fs15CreateDirectoryEPKc(void) {
    _SYM_ABORTF("_ZN2nn2fs15CreateDirectoryEPKc");
    // TODO
}

SYM_SYMBOL u64 _ZN2nn2os13GetSystemTickEv(void) {
    _SYM_LOGF("nn::os::GetSystemTick(void) called!");
    return armGetSystemTick();
}

SYM_SYMBOL u64 _ZN2nn2os17ConvertToTimeSpanENS0_4TickE(u64 tick) {
    _SYM_LOGF("nn::os::ConvertToTimeSpan(nn::os::Tick) called!");

    if ( 1000000000 * (tick % 19200000) / 19200000 + 1000000000 * (tick / 19200000) >= 0 || tick <= 0 )
        return 1000000000 * (tick % 19200000) / 19200000 + 1000000000 * (tick / 19200000);
    else
        return 0x7FFFFFFFFFFFFFFFLL;
}

SYM_SYMBOL Result _ZN2nn4time14ToCalendarTimeEPNS0_12CalendarTimeEPNS0_22CalendarAdditionalInfoERKNS0_9PosixTimeE(TimeCalendarTime *out_calendar_time, TimeCalendarAdditionalInfo *out_additional_info, const u64 in_posix_time) {
    _SYM_LOGF("nn::time::ToCalendarTime(nn::time::CalendarTime *, nn::time::CalendarAdditionalInfo *, nn::time::PosixTime const&) called!");
    return timeToCalendarTimeWithMyRule(in_posix_time, out_calendar_time, out_additional_info);
}

SYM_SYMBOL void _ZN2nn4util8SNPrintfEPcmPKcz(void) {
    _SYM_ABORTF("_ZN2nn4util8SNPrintfEPcmPKcz");
    // TODO
}

SYM_SYMBOL void _ZN2nn2os19AllocateMemoryPagesEmm(void) {
    _SYM_ABORTF("_ZN2nn2os19AllocateMemoryPagesEmm");
    // TODO
}

SYM_SYMBOL void _ZN2nn2os15FreeMemoryPagesEmm(void) {
    _SYM_ABORTF("_ZN2nn2os15FreeMemoryPagesEmm");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket6SocketENS0_6FamilyENS0_4TypeENS0_8ProtocolE(void) {
    _SYM_ABORTF("_ZN2nn6socket6SocketENS0_6FamilyENS0_4TypeENS0_8ProtocolE");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket10SetSockOptEiNS0_5LevelENS0_6OptionEPKvj(void) {
    _SYM_ABORTF("_ZN2nn6socket10SetSockOptEiNS0_5LevelENS0_6OptionEPKvj");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket12GetLastErrorEv(void) {
    _SYM_ABORTF("_ZN2nn6socket12GetLastErrorEv");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket5CloseEi(void) {
    _SYM_ABORTF("_ZN2nn6socket5CloseEi");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket6ListenEii(void) {
    _SYM_ABORTF("_ZN2nn6socket6ListenEii");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket6AcceptEiPNS0_8SockAddrEPj(void) {
    _SYM_ABORTF("_ZN2nn6socket6AcceptEiPNS0_8SockAddrEPj");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket4BindEiPKNS0_8SockAddrEj(void) {
    _SYM_ABORTF("_ZN2nn6socket4BindEiPKNS0_8SockAddrEj");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket7ConnectEiPKNS0_8SockAddrEj(void) {
    _SYM_ABORTF("_ZN2nn6socket7ConnectEiPKNS0_8SockAddrEj");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket8RecvFromEiPvmNS0_7MsgFlagEPNS0_8SockAddrEPj(void) {
    _SYM_ABORTF("_ZN2nn6socket8RecvFromEiPvmNS0_7MsgFlagEPNS0_8SockAddrEPj");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket4RecvEiPvmNS0_7MsgFlagE(void) {
    _SYM_ABORTF("_ZN2nn6socket4RecvEiPvmNS0_7MsgFlagE");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket6SendToEiPKvmNS0_7MsgFlagEPKNS0_8SockAddrEj(void) {
    _SYM_ABORTF("_ZN2nn6socket6SendToEiPKvmNS0_7MsgFlagEPKNS0_8SockAddrEj");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket4SendEiPKvmNS0_7MsgFlagE(void) {
    _SYM_ABORTF("_ZN2nn6socket4SendEiPKvmNS0_7MsgFlagE");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket8ShutdownEiNS0_14ShutdownMethodE(void) {
    _SYM_ABORTF("_ZN2nn6socket8ShutdownEiNS0_14ShutdownMethodE");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket10GetSockOptEiNS0_5LevelENS0_6OptionEPvPj(void) {
    _SYM_ABORTF("_ZN2nn6socket10GetSockOptEiNS0_5LevelENS0_6OptionEPvPj");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket5IoctlEiNS0_12IoctlCommandEPvm(void) {
    _SYM_ABORTF("_ZN2nn6socket5IoctlEiNS0_12IoctlCommandEPvm");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket4PollEPNS0_6PollFdEmi(void) {
    _SYM_ABORTF("_ZN2nn6socket4PollEPNS0_6PollFdEmi");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket6SelectEiPNS0_5FdSetES2_S2_PNS0_7TimeValE(void) {
    _SYM_ABORTF("_ZN2nn6socket6SelectEiPNS0_5FdSetES2_S2_PNS0_7TimeValE");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket11GetSockNameEiPNS0_8SockAddrEPj(void) {
    _SYM_ABORTF("_ZN2nn6socket11GetSockNameEiPNS0_8SockAddrEPj");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket16GetHostEntByNameEPKcPKNS0_14ResolverOptionEm(void) {
    _SYM_ABORTF("_ZN2nn6socket16GetHostEntByNameEPKcPKNS0_14ResolverOptionEm");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket9GetHErrorEv(void) {
    _SYM_ABORTF("_ZN2nn6socket9GetHErrorEv");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket16GetHostEntByAddrEPKvjNS0_6FamilyE(void) {
    _SYM_ABORTF("_ZN2nn6socket16GetHostEntByAddrEPKvjNS0_6FamilyE");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket11GetPeerNameEiPNS0_8SockAddrEPj(void) {
    _SYM_ABORTF("_ZN2nn6socket11GetPeerNameEiPNS0_8SockAddrEPj");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket11GetAddrInfoEPKcS2_PKNS0_8AddrInfoEPPS3_PKNS0_14ResolverOptionEm(void) {
    _SYM_ABORTF("_ZN2nn6socket11GetAddrInfoEPKcS2_PKNS0_8AddrInfoEPPS3_PKNS0_14ResolverOptionEm");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket12FreeAddrInfoEPNS0_8AddrInfoE(void) {
    _SYM_ABORTF("_ZN2nn6socket12FreeAddrInfoEPNS0_8AddrInfoE");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket8InetAtonEPKcPNS0_6InAddrE(void) {
    _SYM_ABORTF("_ZN2nn6socket8InetAtonEPKcPNS0_6InAddrE");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket5FcntlEiNS0_12FcntlCommandEz(void) {
    _SYM_ABORTF("_ZN2nn6socket5FcntlEiNS0_12FcntlCommandEz");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket9InetHtonlEj(void) {
    _SYM_ABORTF("_ZN2nn6socket9InetHtonlEj");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket9InetHtonsEt(void) {
    _SYM_ABORTF("_ZN2nn6socket9InetHtonsEt");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket9InetNtohlEj(void) {
    _SYM_ABORTF("_ZN2nn6socket9InetNtohlEj");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket9InetNtohsEt(void) {
    _SYM_ABORTF("_ZN2nn6socket9InetNtohsEt");
    // TODO
}

SYM_SYMBOL void _ZN2nn6socket8InetNtoaENS0_6InAddrE(void) {
    _SYM_ABORTF("_ZN2nn6socket8InetNtoaENS0_6InAddrE");
    // TODO
}

SYM_SYMBOL void _ZN2nn2os22GetSystemTickFrequencyEv(void) {
    _SYM_ABORTF("_ZN2nn2os22GetSystemTickFrequencyEv");
    // TODO
}

SYM_SYMBOL void _ZN2nn2os11SleepThreadENS_8TimeSpanE(u64 time_span_ns) {
    _SYM_LOGF("nn::os::SleepThread(nn::TimeSpan) called!");
    _SYM_LOGF("Time span: %llu", time_span_ns);
    svcSleepThread(time_span_ns);
}

SYM_SYMBOL void _ZN2nn2os10WaitThreadEPNS0_10ThreadTypeE(void) {
    _SYM_ABORTF("_ZN2nn2os10WaitThreadEPNS0_10ThreadTypeE");
    // TODO
}

SYM_SYMBOL void _ZN2nn2os20GetThreadNamePointerEPKNS0_10ThreadTypeE(void) {
    _SYM_ABORTF("_ZN2nn2os20GetThreadNamePointerEPKNS0_10ThreadTypeE");
    // TODO
}

SYM_SYMBOL void _ZN2nn2os13DestroyThreadEPNS0_10ThreadTypeE(void) {
    _SYM_ABORTF("_ZN2nn2os13DestroyThreadEPNS0_10ThreadTypeE");
    // TODO
}

SYM_SYMBOL void _ZN2nn2os13FinalizeEventEPNS0_9EventTypeE(void) {
    _SYM_ABORTF("_ZN2nn2os13FinalizeEventEPNS0_9EventTypeE");
    // TODO
}

SYM_SYMBOL NnOsThreadType *_ZN2nn2os16GetCurrentThreadEv(void) {
    // _SYM_LOGF("nn::os::GetCurrentThread(void) called!");
    const Handle handle = threadGetCurHandle();
    return FindAllocatedThreadByHandle(handle);
}

SYM_SYMBOL s32 _ZN2nn2os17GetThreadPriorityEPKNS0_10ThreadTypeE(NnOsThreadType *thread) {
    _SYM_LOGF("nn::os::GetThreadPriority(nn::os::ThreadType const*) called!");
    _SYM_LOGF("Thread: %p", thread);
    return thread->BasePriority;
}

SYM_SYMBOL u32 _ZN2nn2os20GetCurrentCoreNumberEv(void) {
    _SYM_LOGF("nn::os::GetCurrentCoreNumber(void) called!");
    return svcGetCurrentProcessorNumber();
}

SYM_SYMBOL u64 _ZN2nn2os26GetThreadAvailableCoreMaskEv(void) {
    _SYM_LOGF("nn::os::GetThreadAvailableCoreMask(void) called!");

    u64 mask;
    svcGetInfo(&mask, InfoType_CoreMask, CUR_PROCESS_HANDLE, 0);
    return mask;
}

typedef struct {
    u8 data[0x28];
} NnOsEventType;

typedef enum {
    NnOsEventClearMode_ManualClear = 0,
    NnOsEventClearMode_AutoClear = 1
} NnOsEventClearMode;

typedef struct {
    UEvent uevent;
} CustomEventType;

_Static_assert(sizeof(NnOsEventType) == 0x28);
_Static_assert(sizeof(CustomEventType) == sizeof(NnOsEventType));

SYM_SYMBOL void _ZN2nn2os15InitializeEventEPNS0_9EventTypeEbNS0_14EventClearModeE(NnOsEventType *event, bool initially_signaled, NnOsEventClearMode clear_mode) {
    _SYM_LOGF("nn::os::InitializeEvent(nn::os::EventType *, bool, nn::os::EventClearMode) called!");
    _SYM_LOGF("Initializing event %p (initially_signaled=%d, clear_mode=%d)", event, initially_signaled, clear_mode);
    // TODO: implement this
    CustomEventType *our_event = (CustomEventType*)event;
    ueventCreate(&our_event->uevent, clear_mode == NnOsEventClearMode_AutoClear);
}

SYM_SYMBOL Result _ZN2nn2os12CreateThreadEPNS0_10ThreadTypeEPFvPvES3_S3_mii(NnOsThreadType *thread, void (*func)(void*), void *arg, void *stack, unsigned long stack_size, int priority, int core) {
    _SYM_LOGF("nn::os::CreateThread(nn::os::ThreadType *, void (*)(void *), void *, void *, unsigned long, int, int) called!");
    _SYM_LOGF("Thread: %p, func: %p, arg: %p, stack: %p, stack_size: %lu, priority: %d, core: %d", thread, func, arg, stack, stack_size, priority, core);

    memset(thread, 0, sizeof(NnOsThreadType));
    R_TRY(threadCreate(&thread->libnx_thread, func, arg, stack, stack_size, priority + 28, core));
    AllocateThread(thread);
    return R_SUCCESS;
}

SYM_SYMBOL void _ZN2nn2os13SetThreadNameEPNS0_10ThreadTypeEPKc(NnOsThreadType *thread, const char *name) {
    _SYM_LOGF("nn::os::SetThreadName(nn::os::ThreadType *, char const*) called!");
    _SYM_LOGF("Thread: %p, name: '%s'", thread, name);
    SetAllocatedThreadName(thread, name);
}

SYM_SYMBOL void _ZN2nn2os11StartThreadEPNS0_10ThreadTypeE(NnOsThreadType *thread) {
    _SYM_LOGF("nn::os::StartThread(nn::os::ThreadType *) called!");
    _SYM_LOGF("Thread: %p", thread);

    Result rc = threadStart(&thread->libnx_thread);
    if(R_SUCCEEDED(rc)) {
        _SYM_LOGF("Thread started successfully");
    }
    else {
        _SYM_LOGF("Thread start failed: 0x%X", rc);
    }
}

SYM_SYMBOL void _ZN2nn2os11SignalEventEPNS0_9EventTypeE(NnOsEventType *event) {
    _SYM_LOGF("nn::os::SignalEvent(nn::os::EventType *) called!");
    _SYM_LOGF("Event: %p", event);

    CustomEventType *our_event = (CustomEventType*)event;
    ueventSignal(&our_event->uevent);
}

SYM_SYMBOL void _ZN2nn2os9WaitEventEPNS0_9EventTypeE(void) {
    _SYM_ABORTF("_ZN2nn2os9WaitEventEPNS0_9EventTypeE");
    // TODO
}

SYM_SYMBOL void _ZN2nn2os11YieldThreadEv(void) {
    _SYM_ABORTF("_ZN2nn2os11YieldThreadEv");
    // TODO
}

SYM_SYMBOL bool _ZN2nn2os12TryLockMutexEPNS0_9MutexTypeE(NnOsMutexType *mutex) {
    _SYM_LOGF("nn::os::TryLockMutex(nn::os::MutexType *) called!");

    CustomMutexType *our_mutex = (CustomMutexType*)mutex;
    if(our_mutex->is_recursive) {
        _SYM_LOGF("Trying to lock recursive mutex %p", mutex);
        return rmutexTryLock(&our_mutex->rmutex);
    }
    else {
        _SYM_LOGF("Trying to lock mutex %p", mutex);
        return mutexTryLock(&our_mutex->mutex);
    }
}

SYM_SYMBOL bool _ZN2nn2os28IsMutexLockedByCurrentThreadEPKNS0_9MutexTypeE(NnOsMutexType *mutex) {
    _SYM_LOGF("nn::os::IsMutexLockedByCurrentThread(nn::os::MutexType const*) called!");
    
    CustomMutexType *our_mutex = (CustomMutexType*)mutex;
    _SYM_LOGF("Mutex: %p, recursive: %d", mutex, our_mutex->is_recursive);

    if(our_mutex->is_recursive) {
        return false;
    }
    return mutexIsLockedByCurrentThread(&our_mutex->mutex);
}

SYM_SYMBOL bool _ZN2nn2os14TimedWaitEventEPNS0_9EventTypeENS_8TimeSpanE(NnOsEventType *event, u64 time_span_ns) {
    _SYM_LOGF("nn::os::TimedWaitEvent(nn::os::EventType *, nn::TimeSpan) called!");
    _SYM_LOGF("Event: %p, time_span_ns: %llu", event, time_span_ns);

    CustomEventType *our_event = (CustomEventType*)event;
    Result rc = waitSingle(waiterForUEvent(&our_event->uevent), time_span_ns);
    if(rc == KERNELRESULT(TimedOut)) {
        _SYM_LOGF("Timed out");
        return false;
    }
    else if(R_FAILED(rc)) {
        _SYM_LOGF("Wait failed: 0x%X", rc);
        return false;
    }
    else {
        _SYM_LOGF("Wait succeeded");
        return true;
    }
}

typedef struct {
    u8 data[0x40];
} NnOsTimerEventType;

typedef struct {
    UTimer utimer;
    bool signal;
    bool auto_clear;
} CustomTimerEventType;

_Static_assert(sizeof(NnOsTimerEventType) == 0x40);
_Static_assert(sizeof(CustomTimerEventType) == sizeof(NnOsTimerEventType));

NX_INLINE UEvent GetUEventFromTimerEvent(CustomTimerEventType *timer_event) {
    const UEvent v = {
        .waitable = timer_event->utimer.waitable,
        .signal = timer_event->signal,
        .auto_clear = timer_event->auto_clear
    };
    return v;
}

NX_INLINE void SetUEventFromTimerEvent(CustomTimerEventType *timer_event, const UEvent *uevent) {
    timer_event->utimer.waitable = uevent->waitable;
    timer_event->signal = uevent->signal;
    timer_event->auto_clear = uevent->auto_clear;
}

SYM_SYMBOL void _ZN2nn2os20InitializeTimerEventEPNS0_14TimerEventTypeENS0_14EventClearModeE(NnOsTimerEventType *timer_event, NnOsEventClearMode clear_mode) {
    _SYM_LOGF("nn::os::InitializeTimerEvent(nn::os::TimerEventType *, nn::os::EventClearMode) called!");

    _SYM_LOGF("Initializing timer event %p (clear_mode=%d)", timer_event, clear_mode);
    CustomTimerEventType *our_timer_event = (CustomTimerEventType*)timer_event;
    utimerCreate(&our_timer_event->utimer, 0, TimerType_OneShot);
}

SYM_SYMBOL void _ZN2nn2os18FinalizeTimerEventEPNS0_14TimerEventTypeE(NnOsTimerEventType *timer_event) {
    _SYM_LOGF("nn::os::FinalizeTimerEvent(nn::os::TimerEventType *) called!");
    _SYM_LOGF("Timer event: %p", timer_event);

    CustomTimerEventType *our_timer_event = (CustomTimerEventType*)timer_event;
    utimerStop(&our_timer_event->utimer);
}

SYM_SYMBOL void _ZN2nn2os16SignalTimerEventEPNS0_14TimerEventTypeE(void) {
    _SYM_ABORTF("_ZN2nn2os16SignalTimerEventEPNS0_14TimerEventTypeE");
    // TODO
}

SYM_SYMBOL void _ZN2nn2os22StartOneShotTimerEventEPNS0_14TimerEventTypeENS_8TimeSpanE(NnOsTimerEventType *timer_event, u64 time_span_ns) {
    _SYM_LOGF("nn::os::StartOneShotTimerEvent(nn::os::TimerEventType *, nn::TimeSpan) called!");
    _SYM_LOGF("Timer event: %p, time_span_ns: %lu", timer_event, time_span_ns);
    CustomTimerEventType *our_timer_event = (CustomTimerEventType*)timer_event;
    our_timer_event->utimer.next_tick = time_span_ns;
    our_timer_event->utimer.interval = 0;
    our_timer_event->utimer.type = TimerType_OneShot;
    utimerStart(&our_timer_event->utimer);
}

SYM_SYMBOL void _ZN2nn2os14WaitTimerEventEPNS0_14TimerEventTypeE(NnOsTimerEventType *timer_event) {
    _SYM_LOGF("nn::os::WaitTimerEvent(nn::os::TimerEventType *) called!");
    _SYM_LOGF("Timer event: %p", timer_event);

    CustomTimerEventType *our_timer_event = (CustomTimerEventType*)timer_event;
    waitSingle(waiterForUTimer(&our_timer_event->utimer), UINT64_MAX);
}

///////////////////////////////////////////////////////////////////////////////////////////

void symInitialize(void) {
    // Set valid values to our fake stdout/stderr/stdin symbols
    stdout = __getreent()->_stdout;
    stderr = __getreent()->_stderr;
    stdin = __getreent()->_stdin;

    // Prepare the main thread
    Thread *main_thread = threadGetSelf();
    NnOsThreadType *fake_main_thread = (NnOsThreadType*)malloc(sizeof(NnOsThreadType));
    memset(fake_main_thread, 0, sizeof(NnOsThreadType));
    fake_main_thread->libnx_thread = *main_thread;
    AllocateThread(fake_main_thread);
    SetAllocatedThreadName(fake_main_thread, "MainFakeThread");
}

void symExit(void) {
    // Free our fake main thread
    NnOsThreadType *fake_main_thread = g_ThreadList[0];
    free(fake_main_thread);
    for(u32 i = 0; i < g_ThreadListCount; i++) {
        if(i > 0) {
            NnOsThreadType *thread = g_ThreadList[i];
            threadPause(&thread->libnx_thread);
            threadClose(&thread->libnx_thread);
        }
        g_ThreadList[i] = NULL;
    }
    g_ThreadListCount = 0;
}
