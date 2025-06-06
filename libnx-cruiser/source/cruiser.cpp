#include <switch-ld.h>
extern "C" {
    #include "cruiser/cruiser/wkc.h"
    #include "cruiser/cruiser/jsc.h"
}
#include "cruiser/cruiser-cpp/webkit.hpp"

#define _CRUISEEXT_R_TRY_LOAD_SYMBOL(name, out_sym) { \
    void *sym; \
    const auto rc = ldLookupGlobalSymbol(name, false, &sym); \
    if(R_FAILED(rc)) { \
        extLogf("[cruiser] Failed to load symbol %s: 0x%X", name, rc); \
        return rc; \
    } \
    out_sym = (decltype(out_sym))sym; \
}

extern "C" Result cruiserInitialize(void) {
    // WKC
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("wkcSetReportArrowFunctionCallbackPeer", wkcSetReportArrowFunctionCallbackPeer);
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("wkcSetReportArrowFunctionThisCallbackPeer", wkcSetReportArrowFunctionThisCallbackPeer);

    // JSC
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("JSGlobalContextCreate", JSGlobalContextCreate);
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("JSStringCreateWithUTF8CString", JSStringCreateWithUTF8CString);
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("JSEvaluateScript", JSEvaluateScript);
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("JSValueToNumber", JSValueToNumber);
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("JSStringRelease", JSStringRelease);
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("JSGlobalContextRelease", JSGlobalContextRelease);

    // WebKit
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("_ZN3WKC19WKCWebKitInitializeEPvmmS0_mRNS_21WKCMemoryEventHandlerERNS_20WKCTimerEventHandlerE", WKC::WKCWebKitInitialize);
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("_ZN3WKC8WKCPrefs13setThreadInfoEPvS1_", WKC::WKCPrefs::setThreadInfo);
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("_ZN3WKC8WKCPrefs12setStackSizeEj", WKC::WKCPrefs::setStackSize);
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("_ZN3WKC8WKCPrefs16setSystemStringsEPKNS_14SystemStrings_E", WKC::WKCPrefs::setSystemStrings);
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("_ZN3WKC28WKCWebKitSetMediaPlayerProcsEPK20WKCMediaPlayerProcs_", WKC::WKCWebKitSetMediaPlayerProcs);
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("_ZN3WKC23WKCWebKitSetThreadProcsEPK15WKCThreadProcs_", WKC::WKCWebKitSetThreadProcs);
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("_ZN3WKC4Heap16GetAvailableSizeEv", WKC::Heap::GetAvailableSize);
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("_ZN3WKC4Heap24GetMaxAvailableBlockSizeEv", WKC::Heap::GetMaxAvailableBlockSize);
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("_ZN3WKC4Heap32SetJSRegisterFileDefaultCapacityEj", WKC::Heap::SetJSRegisterFileDefaultCapacity);
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("_ZN3WKC4Heap28GetJSHeapAllocatedBlockBytesEv", WKC::Heap::GetJSHeapAllocatedBlockBytes);
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("_ZN3WKC4Heap27GetStatisticsFreeSizeInHeapEv", WKC::Heap::GetStatisticsFreeSizeInHeap);
    _CRUISEEXT_R_TRY_LOAD_SYMBOL("_ZN3WKC4Heap13GetStatisticsERNS0_11Statistics_Em", WKC::Heap::GetStatistics);
    return EXT_R_SUCCESS;
}

extern "C" void cruiserExit(void) {
    // Nothing to do
}

