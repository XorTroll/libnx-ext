#include "log.hpp"
#include <switch-cruiser.h>
#include <stdlib.h>

PadState pad;

NX_NORETURN void finish() {
    cruiserExit();
    ossExit();

    while(appletMainLoop()) {
        padUpdate(&pad);

        u64 keys_down = padGetButtonsDown(&pad);
        if(keys_down & HidNpadButton_Plus) {
            break;
        }  
    }

    consoleExit(NULL);
    exit(0);
}

#define R_ASSERT(expr) ({ \
    const Result rc = R_VALUE(expr); \
    if(R_FAILED(rc)) { \
        extLogf(#expr " failed with result 0x%X -- %04d-%04d!", rc, 2000 + R_MODULE(rc), R_DESCRIPTION(rc)); \
        finish(); \
    } \
    else { \
        extLogf(#expr " succeeded!"); \
    } \
})

class CustomMemoryEventHandler : public WKC::WKCMemoryEventHandler {
public:
    bool checkMemoryAvailability(size_t request_size, bool in_forimage) override {
        extLogf("[main-cb] checkMemoryAvailability: %zu, %d", request_size, in_forimage);
        return true;
    }

    bool checkMemoryAllocatable(size_t request_size, WKC::WKCMemoryEventHandler::AllocationReason reason) override {
        extLogf("[main-cb] checkMemoryAllocatable: %zu, %d", request_size, reason);
        return true;
    }

    void* notifyMemoryExhaust(size_t request_size, size_t& out_allocated_size) override {
        extLogf("[main-cb] notifyMemoryExhaust: %zu", request_size);
        return nullptr;
    }

    void notifyMemoryAllocationError(size_t request_size, AllocationReason reason) override {
        extLogf("[main-cb] notifyMemoryAllocationError: %zu, %d", request_size, reason);
    }

    void notifyCrash(const char* file, int line, const char* function, const char* assertion) override {
        extLogf("[main-cb] notifyCrash: at %s:%d, function %s: '%s' failed", file, line, function, assertion);
    }

    void notifyStackOverflow(bool need_restart, size_t stack_size, size_t consumption, size_t margin, void* stack_top, void* stack_base, void* current_stack_top, const char* file, int line, const char* function) override {
        extLogf("[main-cb] notifyStackOverflow: %d, %zu, %zu, %zu, %p, %p, %p, %s:%d:%s", need_restart, stack_size, consumption, margin, stack_top, stack_base, current_stack_top, file, line, function);
    }

    bool allocateMemoryPages(void* ptr, size_t size) override {
        extLogf("[main-cb] allocateMemoryPages: %p, %zu", ptr, size);
        return true;
    }

    void freeMemoryPages(void* ptr, size_t size) override {
        extLogf("[main-cb] freeMemoryPages: %p, %zu", ptr, size);
    }
};

class CustomTimerEventHandler : public WKC::WKCTimerEventHandler {
public:
    bool requestWakeUp(void* in_timer, bool(*in_proc)(void*), void* in_data) override {
        extLogf("[main-cb] requestWakeUp: %p, %p, %p", in_timer, in_proc, in_data);
        return true;
    }

    void cancelWakeUp(void* in_timer) override {
        extLogf("[main-cb] cancelWakeUp: %p", in_timer);
    }
};

void ReportArrowFunctionThisCallback() {
    extLogf("[main-cb] ReportArrowFunctionThisCallbackPeer()");
}

void ReportArrowFunctionCallback() {
    extLogf("[main-cb] ReportArrowFunctionCallbackPeer()");
}

bool test_wkcMediaPlayerInitialize(void) {
    extLogf("[main-cb] wkcMediaPlayerInitialize()");
    return true;
}
void test_wkcMediaPlayerFinalize(void) {
    extLogf("[main-cb] wkcMediaPlayerFinalize()");
}
void test_wkcMediaPlayerForceTerminate(void) {
    extLogf("[main-cb] wkcMediaPlayerForceTerminate()");
}
int test_wkcMediaPlayerIsSupportedMIMEType(const char* in_mimetype, const char* in_codec, const char* in_system, const char* in_url) {
    extLogf("[main-cb] wkcMediaPlayerIsSupportedMIMEType(%s, %s, %s, %s)", in_mimetype, in_codec, in_system, in_url);
    return 0;
}
int test_wkcMediaPlayerSupportedMIMETypes(void) {
    extLogf("[main-cb] wkcMediaPlayerSupportedMIMETypes()");
    return 0;
}
const char* test_wkcMediaPlayerSupportedMIMEType(int in_index) {
    extLogf("[main-cb] wkcMediaPlayerSupportedMIMEType(%d)", in_index);
    return NULL;
}
const char* test_wkcMediaPlayerGetUIString(int in_type) {
    extLogf("[main-cb] wkcMediaPlayerGetUIString(%d)", in_type);
    return NULL;
}
const char* test_wkcMediaPlayerGetUIStringTime(float in_time) {
    extLogf("[main-cb] wkcMediaPlayerGetUIStringTime(%f)", in_time);
    return NULL;
}
void* test_wkcMediaPlayerCreate(const WKCMediaPlayerCallbacks* in_callbacks, void* in_opaque) {
    extLogf("[main-cb] wkcMediaPlayerCreate(%p, %p)", in_callbacks, in_opaque);
    return NULL;
}
void test_wkcMediaPlayerDelete(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerDelete(%p)", in_self);
}
int test_wkcMediaPlayerLoad(void* in_self, const char* in_uri, int in_mediatype) {
    extLogf("[main-cb] wkcMediaPlayerLoad(%p, %s, %d)", in_self, in_uri, in_mediatype);
    return 0;
}
int test_wkcMediaPlayerCancelLoad(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerCancelLoad(%p)", in_self);
    return 0;
}
int test_wkcMediaPlayerPrepareToPlay(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerPrepareToPlay(%p)", in_self);
    return 0;
}
int test_wkcMediaPlayerPlay(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerPlay(%p)", in_self);
    return 0;
}
int test_wkcMediaPlayerPause(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerPause(%p)", in_self);
    return 0;
}
bool test_wkcMediaPlayerIsPaused(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerIsPaused(%p)", in_self);
    return false;
}
bool test_wkcMediaPlayerHasVideo(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerHasVideo(%p)", in_self);
    return false;
}
bool test_wkcMediaPlayerHasAudio(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerHasAudio(%p)", in_self);
    return false;
}
bool test_wkcMediaPlayerSupportsFullScreen(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerSupportsFullScreen(%p)", in_self);
    return false;
}
bool test_wkcMediaPlayerCanSaveMediaData(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerCanSaveMediaData(%p)", in_self);
    return false;
}
bool test_wkcMediaPlayerHasClosedCaptions(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerHasClosedCaptions(%p)", in_self);
    return false;
}
int test_wkcMediaPlayerSetClosedCaptionsVisible(void* in_self, bool in_flag) {
    extLogf("[main-cb] wkcMediaPlayerSetClosedCaptionsVisible(%p, %d)", in_self, in_flag);
    return 0;
}
bool test_wkcMediaPlayerHasSingleSecurityOrigin(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerHasSingleSecurityOrigin(%p)", in_self);
    return false;
}
int test_wkcMediaPlayerNaturalSize(void* in_self, WKCSize* out_size) {
    extLogf("[main-cb] wkcMediaPlayerNaturalSize(%p, %p)", in_self, out_size);
    return 0;
}
int test_wkcMediaPlayerSetSize(void* in_self, const WKCSize* in_size) {
    extLogf("[main-cb] wkcMediaPlayerSetSize(%p, %p)", in_self, in_size);
    return 0;
}
int test_wkcMediaPlayerSetVisible(void* in_self, bool in_flag) {
    extLogf("[main-cb] wkcMediaPlayerSetVisible(%p, %d)", in_self, in_flag);
    return 0;
}
double test_wkcMediaPlayerDuration(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerDuration(%p)", in_self);
    return 0.0;
}
float test_wkcMediaPlayerStartTime(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerStartTime(%p)", in_self);
    return 0.0f;
}
double test_wkcMediaPlayerCurrentTime(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerCurrentTime(%p)", in_self);
    return 0.0;
}
float test_wkcMediaPlayerMaxTimeSeekable(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerMaxTimeSeekable(%p)", in_self);
    return 0.0f;
}
int test_wkcMediaPlayerSeek(void* in_self, float in_time) {
    extLogf("[main-cb] wkcMediaPlayerSeek(%p, %f)", in_self, in_time);
    return 0;
}
double test_wkcMediaPlayerRate(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerRate(%p)", in_self);
    return 0.0;
}
int test_wkcMediaPlayerSetRate(void* in_self, float in_rate) {
    extLogf("[main-cb] wkcMediaPlayerSetRate(%p, %f)", in_self, in_rate);
    return 0;
}
int test_wkcMediaPlayerSetPreservesPitch(void* in_self, bool in_flag) {
    extLogf("[main-cb] wkcMediaPlayerSetPreservesPitch(%p, %d)", in_self, in_flag);
    return 0;
}
float test_wkcMediaPlayerVolume(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerVolume(%p)", in_self);
    return 0.0f;
}
int test_wkcMediaPlayerSetVolume(void* in_self, float in_volume) {
    extLogf("[main-cb] wkcMediaPlayerSetVolume(%p, %f)", in_self, in_volume);
    return 0;
}
int test_wkcMediaPlayerNetworkState(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerNetworkState(%p)", in_self);
    return 0;
}
int test_wkcMediaPlayerReadyState(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerReadyState(%p)", in_self);
    return 0;
}
int test_wkcMediaPlayerMovieLoadType(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerMovieLoadType(%p)", in_self);
    return 0;
}
int test_wkcMediaPlayerSetWindowPosition(void* in_self, const WKCRect* in_rect) {
    extLogf("[main-cb] wkcMediaPlayerSetWindowPosition(%p, %p)", in_self, in_rect);
    return 0;
}
int test_wkcMediaPlayerVideoSinkType(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerVideoSinkType(%p)", in_self);
    return 0;
}
int test_wkcMediaPlayerBufferedRanges(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerBufferedRanges(%p)", in_self);
    return 0;
}
int test_wkcMediaPlayerBufferedRange(void* in_self, int in_index, float* out_start, float* out_end) {
    extLogf("[main-cb] wkcMediaPlayerBufferedRange(%p, %d, %p, %p)", in_self, in_index, out_start, out_end);
    return 0;
}
void test_wkcMediaPlayerRemovedFromDocument(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerRemovedFromDocument(%p)", in_self);
}
void test_wkcMediaPlayerWillBecomeInactive(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerWillBecomeInactive(%p)", in_self);
}
void test_wkcMediaPlayerEnterFullscreen(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerEnterFullscreen(%p)", in_self);
}
void test_wkcMediaPlayerExitFullscreen(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerExitFullscreen(%p)", in_self);
}
bool test_wkcMediaPlayerCanEnterFullscreen(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerCanEnterFullscreen(%p)", in_self);
    return false;
}
int test_wkcMediaPlayerDecodeAudioData(const void* in_data, int in_len, void* out_buf, int in_buflen, int* out_samplerate, int* out_bitspersample, int* out_channels, int* out_endian) {
    extLogf("[main-cb] wkcMediaPlayerDecodeAudioData(%p, %d, %p, %d, %p, %p, %p, %p)", in_data, in_len, out_buf, in_buflen, out_samplerate, out_bitspersample, out_channels, out_endian);
    return 0;
}
int test_wkcMediaPlayerAddSB(void* in_self, const char* in_type, void** out_sourcebuffer) {
    extLogf("[main-cb] wkcMediaPlayerAddSB(%p, %s, %p)", in_self, in_type, out_sourcebuffer);
    return 0;
}
double test_wkcMediaPlayerGetMSDuration(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerGetMSDuration(%p)", in_self);
    return 0.0;
}
int test_wkcMediaPlayerSetMSDuration(void* in_self, double in_duration) {
    extLogf("[main-cb] wkcMediaPlayerSetMSDuration(%p, %f)", in_self, in_duration);
    return 0;
}
int test_wkcMediaPlayerNotifyMSMarkEndOfStream(void* in_self, int in_status) {
    extLogf("[main-cb] wkcMediaPlayerNotifyMSMarkEndOfStream(%p, %d)", in_self, in_status);
    return 0;
}
int test_wkcMediaPlayerNotifyMSUnmarkEndOfStream(void* in_self) {
    extLogf("[main-cb] wkcMediaPlayerNotifyMSUnmarkEndOfStream(%p)", in_self);
    return 0;
}
int test_wkcMediaPlayerSetReadyState(void* in_self, int in_state) {
    extLogf("[main-cb] wkcMediaPlayerSetReadyState(%p, %d)", in_self, in_state);
    return 0;
}
int test_wkcMediaPlayerAppendToSB(void* in_self, void* in_sourcebuffer, const unsigned char* in_data, unsigned in_length, const WKCMediaPlayerSourceBufferCallbacks* in_callbacks, void* in_opaque) {
    extLogf("[main-cb] wkcMediaPlayerAppendToSB(%p, %p, %p, %u, %p, %p)", in_self, in_sourcebuffer, in_data, in_length, in_callbacks, in_opaque);
    return 0;
}
int test_wkcMediaPlayerAbortSB(void* in_self, void* in_sourcebuffer) {
    extLogf("[main-cb] wkcMediaPlayerAbortSB(%p, %p)", in_self, in_sourcebuffer);
    return 0;
}
bool test_wkcMediaPlayerSetSBTimestampOffset(void* in_self, void* in_sourcebuffer, double in_timestamp_offset) {
    extLogf("[main-cb] wkcMediaPlayerSetSBTimestampOffset(%p, %p, %f)", in_self, in_sourcebuffer, in_timestamp_offset);
    return 0;
}
int test_wkcMediaPlayerRemoveSB(void* in_self, void* in_sourcebuffer) {
    extLogf("[main-cb] wkcMediaPlayerRemoveSB(%p, %p)", in_self, in_sourcebuffer);
    return 0;
}
int test_wkcMediaPlayerRemoveCodedFrames(void* in_self, void* in_sourcebuffer, double in_start, double in_end) {
    extLogf("[main-cb] wkcMediaPlayerRemoveCodedFrames(%p, %p, %f, %f)", in_self, in_sourcebuffer, in_start, in_end);
    return 0;
}
int test_wkcMediaPlayerEvictCodedFramesSB(void* in_self, void* in_sourcebuffer, double* out_start, double* out_end) {
    extLogf("[main-cb] wkcMediaPlayerEvictCodedFramesSB(%p, %p, %p, %p)", in_self, in_sourcebuffer, out_start, out_end);
    return 0;
}
bool test_wkcMediaPlayerIsFullSB(void* in_self, void* in_sourcebuffer) {
    extLogf("[main-cb] wkcMediaPlayerIsFullSB(%p, %p)", in_self, in_sourcebuffer);
    return false;
}

WKCMediaPlayerProcs g_media_player_procs = {
    .fInitializeProc = test_wkcMediaPlayerInitialize,
    .fFinalizeProc = test_wkcMediaPlayerFinalize,
    .fForceTerminateProc = test_wkcMediaPlayerForceTerminate,
    .fIsSupportedMIMETypeProc = test_wkcMediaPlayerIsSupportedMIMEType,
    .fSupportedMIMETypesProc = test_wkcMediaPlayerSupportedMIMETypes,
    .fSupportedMIMETypeProc = test_wkcMediaPlayerSupportedMIMEType,
    .fGetUIStringProc = test_wkcMediaPlayerGetUIString,
    .fGetUIStringTimeProc = test_wkcMediaPlayerGetUIStringTime,
    .fCreateProc = test_wkcMediaPlayerCreate,
    .fDeleteProc = test_wkcMediaPlayerDelete,
    .fLoadProc = test_wkcMediaPlayerLoad,
    .fCancelLoadProc = test_wkcMediaPlayerCancelLoad,
    .fPrepareToPlayProc = test_wkcMediaPlayerPrepareToPlay,
    .fPlayProc = test_wkcMediaPlayerPlay,
    .fPauseProc = test_wkcMediaPlayerPause,
    .fIsPausedProc = test_wkcMediaPlayerIsPaused,
    .fHasVideoProc = test_wkcMediaPlayerHasVideo,
    .fHasAudioProc = test_wkcMediaPlayerHasAudio,
    .fSupportsFullScreenProc = test_wkcMediaPlayerSupportsFullScreen,
    .fCanSaveMediaDataProc = test_wkcMediaPlayerCanSaveMediaData,
    .fHasClosedCaptionsProc = test_wkcMediaPlayerHasClosedCaptions,
    .fSetClosedCaptionsVisibleProc = test_wkcMediaPlayerSetClosedCaptionsVisible,
    .fHasSingleSecurityOriginProc = test_wkcMediaPlayerHasSingleSecurityOrigin,
    .fNaturalSizeProc = test_wkcMediaPlayerNaturalSize,
    .fSetSizeProc = test_wkcMediaPlayerSetSize,
    .fSetVisibleProc = test_wkcMediaPlayerSetVisible,
    .fDurationProc = test_wkcMediaPlayerDuration,
    .fStartTimeProc = test_wkcMediaPlayerStartTime,
    .fCurrentTimeProc = test_wkcMediaPlayerCurrentTime,
    .fMaxTimeSeekableProc = test_wkcMediaPlayerMaxTimeSeekable,
    .fSeekProc = test_wkcMediaPlayerSeek,
    .fRateProc = test_wkcMediaPlayerRate,
    .fSetRateProc = test_wkcMediaPlayerSetRate,
    .fSetPreservesPitchProc = test_wkcMediaPlayerSetPreservesPitch,
    .fVolumeProc = test_wkcMediaPlayerVolume,
    .fSetVolumeProc = test_wkcMediaPlayerSetVolume,
    .fNetworkStateProc = test_wkcMediaPlayerNetworkState,
    .fReadyStateProc = test_wkcMediaPlayerReadyState,
    .fMovieLoadTypeProc = test_wkcMediaPlayerMovieLoadType,
    .fSetWindowPositionProc = test_wkcMediaPlayerSetWindowPosition,
    .fVideoSinkTypeProc = test_wkcMediaPlayerVideoSinkType,
    .fBufferedRangesProc = test_wkcMediaPlayerBufferedRanges,
    .fBufferedRangeProc = test_wkcMediaPlayerBufferedRange,
    .fRemovedFromDocumentProc = test_wkcMediaPlayerRemovedFromDocument,
    .fWillBecomeInactiveProc = test_wkcMediaPlayerWillBecomeInactive,
    .fEnterFullscreenProc = test_wkcMediaPlayerEnterFullscreen,
    .fExitFullscreenProc = test_wkcMediaPlayerExitFullscreen,
    .fCanEnterFullscreenProc = test_wkcMediaPlayerCanEnterFullscreen,
    .fDecodeAudioDataProc = test_wkcMediaPlayerDecodeAudioData,
    .fAddSBProc = test_wkcMediaPlayerAddSB,
    .fGetMSDurationProc = test_wkcMediaPlayerGetMSDuration,
    .fSetMSDurationProc = test_wkcMediaPlayerSetMSDuration,
    .fNotifyMSMarkEndOfStreamProc = test_wkcMediaPlayerNotifyMSMarkEndOfStream,
    .fNotifyMSUnmarkEndOfStreamProc = test_wkcMediaPlayerNotifyMSUnmarkEndOfStream,
    .fSetReadyStateProc = test_wkcMediaPlayerSetReadyState,
    .fAppendToSBProc = test_wkcMediaPlayerAppendToSB,
    .fAbortSBProc = test_wkcMediaPlayerAbortSB,
    .fSetSBTimestampOffsetProc = test_wkcMediaPlayerSetSBTimestampOffset,
    .fRemoveSBProc = test_wkcMediaPlayerRemoveSB,
    .fRemoveCodedFramesProc = test_wkcMediaPlayerRemoveCodedFrames,
    .fEvictCodedFramesSBProc = test_wkcMediaPlayerEvictCodedFramesSB,
    .fIsFullSBProc = test_wkcMediaPlayerIsFullSB
};

bool test_wkcThreadWillCreateThreadProc(const void* in_thread_id, const char* in_name, int* inout_priority, int* inout_core_number, unsigned int* inout_stack_size) {
    extLogf("[main-cb] wkcThreadWillCreateThreadProc(%p, %s, %d, %d, %u)", in_thread_id, in_name, *inout_priority, *inout_core_number, *inout_stack_size);
    return true;
}
void test_wkcThreadDidCreateThreadProc(const void* in_thread_id, void* in_native_thread_handle, const char* in_name, int in_priority, int in_core_number, unsigned int in_stack_size) {
    extLogf("[main-cb] wkcThreadDidCreateThreadProc(%p, %p, %s, %d, %d, %u)", in_thread_id, in_native_thread_handle, in_name, in_priority, in_core_number, in_stack_size);
}
void test_wkcThreadDidDestroyThreadProc(const void* in_thread_id, void* in_native_thread_handle) {
    extLogf("[main-cb] wkcThreadDidDestroyThreadProc(%p, %p)", in_thread_id, in_native_thread_handle);
}
/* OfflineWeb does not set these
void* test_wkcThreadAllocThreadStackProc(unsigned int in_stacksize) {
    extLogf("[main-cb] wkcThreadAllocThreadStackProc(%u)", in_stacksize);
    return nullptr;
}
void test_wkcThreadFreeThreadStackProc(void* in_stack_address) {
    extLogf("[main-cb] wkcThreadFreeThreadStackProc(%p)", in_stack_address);
}
*/

WKCThreadProcs g_thread_procs = {
    .fWillCreateThreadProc = test_wkcThreadWillCreateThreadProc,
    .fDidCreateThreadProc = test_wkcThreadDidCreateThreadProc,
    .fDidDestroyThreadProc = test_wkcThreadDidDestroyThreadProc,
    .fAllocThreadStackProc = nullptr,
    .fFreeThreadStackProc = nullptr
};

////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
    consoleInit(NULL);
    extSetLogFunction(customLog);

    // Redirect stderr, since WTF/WKC may log there
    FILE *new_stderr_log = freopen("sdmc:/cruiser-wtf-stderr.log", "w", stderr);
    if(new_stderr_log == nullptr) {
        extLogf("Failed to open stderr redir file!");
        finish();
    }

    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);

    // Mounts shared data and loads all OSS/WKC NRO modules
    R_ASSERT(ossInitialize(OssBrowserDllKind_CfiDisabled));

    // Initializes the symbols/headers
    R_ASSERT(cruiserInitialize());

    extLogf("Initialized cruiser! Initializing WKC...");

    //////////////////////////// Actual WKC init code ////////////////////////////

    // Config before WKCWebKitInitialize

    WKC::WKCPrefs::setThreadInfo(nullptr, threadGetSelf()->stack_mirror);
    WKC::WKCPrefs::setStackSize(threadGetSelf()->stack_sz);

    wkcSetReportArrowFunctionThisCallbackPeer(ReportArrowFunctionThisCallback);
    wkcSetReportArrowFunctionCallbackPeer(ReportArrowFunctionCallback);

    WKC::WKCWebKitSetMediaPlayerProcs(&g_media_player_procs);
    WKC::WKCWebKitSetThreadProcs(&g_thread_procs);

    WKC::Heap::SetJSRegisterFileDefaultCapacity(0x8000);

    CustomMemoryEventHandler memory_event_handler;
    CustomTimerEventHandler timer_event_handler;

    setenv("JSC_verboseCompilation", "yes", 1);
    setenv("JSC_verboseExecutableAllocationFuzz", "yes", 1);
    setenv("JSC_useJIT", "no", 1);
    setenv("JSC_useLLInt", "yes", 1);
    setenv("JSC_verboseSanitizeStack", "yes", 1);
    setenv("JSC_logGC", "yes", 1);
    setenv("JSC_useDollarVM", "yes", 1);

    // WebKitInitialize parameters

    size_t size_1mb = 0x100000;
    size_t memory_size = size_1mb * 32;
    size_t physical_memory_size = memory_size;
    void *memory = __libnx_aligned_alloc(PAGE_ALIGN, memory_size);
    if(memory == nullptr) {
        extLogf("Failed to allocate memory!");
        finish();
    }

    bool wkc_ok = WKC::WKCWebKitInitialize(memory, physical_memory_size, 0, nullptr, 0, memory_event_handler, timer_event_handler);
    extLogf("WKCWebKitInitialize: %d", wkc_ok);
    if(!wkc_ok) {
        extLogf("WKCWebKitInitialize failed!");
        free(memory);
        finish();
    }

    extLogf("WKC initialized! Available size: %zu, Max available block size: %zu, JS allocated block size: %zu", WKC::Heap::GetAvailableSize(), WKC::Heap::GetMaxAvailableBlockSize(), WKC::Heap::GetJSHeapAllocatedBlockBytes());

    extLogf("Running JSC test code...");

    const char *code = "(1 + 2) * 3";
    JSStringRef script = JSStringCreateWithUTF8CString(code);
    extLogf("Created script of code '%s': %p", code, script);

    // Code currently reaches here, but crashes when creating the context (check stderr logs) for some yet unknown reason
    JSGlobalContextRef ctx = JSGlobalContextCreate(nullptr);
    extLogf("Created JS context: %p", ctx);

    JSValueRef exception = nullptr;
    void *result = JSEvaluateScript(ctx, script, nullptr, nullptr, 0, &exception);
    if(exception != nullptr) {
        extLogf("JSEvaluateScript exception: %p", exception);
    }
    else {
        double value = JSValueToNumber(ctx, result, &exception);
        if(exception != nullptr) {
            extLogf("JSValueToNumber exception: %p", exception);
        }
        else {
            extLogf("Result: %f", value);
        }
    }

    // Cleanup
    JSStringRelease(script);
    JSGlobalContextRelease(ctx);
    extLogf("Finished JSC test code!");

    extLogf("Finalizing WKC...");

    WKC::WKCWebKitFinalize();
    free(memory);

    extLogf("WKC finalized!");

    finish();
}
