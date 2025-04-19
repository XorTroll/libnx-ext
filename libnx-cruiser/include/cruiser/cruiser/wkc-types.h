
#pragma once

// https://github.com/syvb/switch-oss/blob/b7f486be1210b9f6d216d334e472b56bee818923/WKC_2.26.0/include/wkc/wkcbase.h

typedef struct WKCSize_ {
    int fWidth;
    int fHeight;
} WKCSize;

typedef struct WKCRect_ {
    int fX;
    int fY;
    int fWidth;
    int fHeight;
} WKCRect;

// https://github.com/syvb/switch-oss/blob/master/WKC_2.26.0/include/wkc/wkcmediaprocs.h

typedef struct WKCMediaPlayerCallbacks_ WKCMediaPlayerCallbacks;
typedef struct WKCMediaPlayerSourceBufferCallbacks_ WKCMediaPlayerSourceBufferCallbacks;

typedef bool (*wkcMediaPlayerInitializeProc)(void);
typedef void (*wkcMediaPlayerFinalizeProc)(void);
typedef void (*wkcMediaPlayerForceTerminateProc)(void);
typedef int (*wkcMediaPlayerIsSupportedMIMETypeProc)(const char* in_mimetype, const char* in_codec, const char* in_system, const char* in_url);
typedef int (*wkcMediaPlayerSupportedMIMETypesProc)(void);
typedef const char* (*wkcMediaPlayerSupportedMIMETypeProc)(int in_index);
typedef const char* (*wkcMediaPlayerGetUIStringProc)(int in_type);
typedef const char* (*wkcMediaPlayerGetUIStringTimeProc)(float in_time);
typedef void* (*wkcMediaPlayerCreateProc)(const WKCMediaPlayerCallbacks* in_callbacks, void* in_opaque);
typedef void (*wkcMediaPlayerDeleteProc)(void* in_self);
typedef int (*wkcMediaPlayerLoadProc)(void* in_self, const char* in_uri, int in_mediatype);
typedef int (*wkcMediaPlayerCancelLoadProc)(void* in_self);
typedef int (*wkcMediaPlayerPrepareToPlayProc)(void* in_self);
typedef int (*wkcMediaPlayerPlayProc)(void* in_self);
typedef int (*wkcMediaPlayerPauseProc)(void* in_self);
typedef bool (*wkcMediaPlayerIsPausedProc)(void* in_self);
typedef bool (*wkcMediaPlayerHasVideoProc)(void* in_self);
typedef bool (*wkcMediaPlayerHasAudioProc)(void* in_self);
typedef bool (*wkcMediaPlayerSupportsFullScreenProc)(void* in_self);
typedef bool (*wkcMediaPlayerCanSaveMediaDataProc)(void* in_self);
typedef bool (*wkcMediaPlayerHasClosedCaptionsProc)(void* in_self);
typedef int (*wkcMediaPlayerSetClosedCaptionsVisibleProc)(void* in_self, bool in_flag);
typedef bool (*wkcMediaPlayerHasSingleSecurityOriginProc)(void* in_self);
typedef int (*wkcMediaPlayerNaturalSizeProc)(void* in_self, WKCSize* out_size);
typedef int (*wkcMediaPlayerSetSizeProc)(void* in_self, const WKCSize* in_size);
typedef int (*wkcMediaPlayerSetVisibleProc)(void* in_self, bool in_flag);
typedef double (*wkcMediaPlayerDurationProc)(void* in_self);
typedef float (*wkcMediaPlayerStartTimeProc)(void* in_self);
typedef double (*wkcMediaPlayerCurrentTimeProc)(void* in_self);
typedef float (*wkcMediaPlayerMaxTimeSeekableProc)(void* in_self);
typedef int (*wkcMediaPlayerSeekProc)(void* in_self, float in_time);
typedef double (*wkcMediaPlayerRateProc)(void* in_self);
typedef int (*wkcMediaPlayerSetRateProc)(void* in_self, float in_rate);
typedef int (*wkcMediaPlayerSetPreservesPitchProc)(void* in_self, bool in_flag);
typedef float (*wkcMediaPlayerVolumeProc)(void* in_self);
typedef int (*wkcMediaPlayerSetVolumeProc)(void* in_self, float in_volume);
typedef int (*wkcMediaPlayerNetworkStateProc)(void* in_self);
typedef int (*wkcMediaPlayerReadyStateProc)(void* in_self);
typedef int (*wkcMediaPlayerMovieLoadTypeProc)(void* in_self);
typedef int (*wkcMediaPlayerSetWindowPositionProc)(void* in_self, const WKCRect* in_rect);
typedef int (*wkcMediaPlayerVideoSinkTypeProc)(void* in_self);
typedef int (*wkcMediaPlayerBufferedRangesProc)(void* in_self);
typedef int (*wkcMediaPlayerBufferedRangeProc)(void* in_self, int in_index, float* out_start, float* out_end);
typedef void (*wkcMediaPlayerRemovedFromDocumentProc)(void* in_self);
typedef void (*wkcMediaPlayerWillBecomeInactiveProc)(void* in_self);
typedef void (*wkcMediaPlayerEnterFullscreenProc)(void* in_self);
typedef void (*wkcMediaPlayerExitFullscreenProc)(void* in_self);
typedef bool (*wkcMediaPlayerCanEnterFullscreenProc)(void* in_self);
typedef int (*wkcMediaPlayerDecodeAudioDataProc)(const void* in_data, int in_len, void* out_buf, int in_buflen, int* out_samplerate, int* out_bitspersample, int* out_channels, int* out_endian);
typedef int (*wkcMediaPlayerAddSBProc)(void* in_self, const char* in_type, void** out_sourcebuffer);
typedef double (*wkcMediaPlayerGetMSDurationProc)(void* in_self);
typedef int (*wkcMediaPlayerSetMSDurationProc)(void* in_self, double in_duration);
typedef int (*wkcMediaPlayerNotifyMSMarkEndOfStreamProc)(void* in_self, int in_status);
typedef int (*wkcMediaPlayerNotifyMSUnmarkEndOfStreamProc)(void* in_self);
typedef int (*wkcMediaPlayerSetReadyStateProc)(void* in_self, int in_state);
typedef int (*wkcMediaPlayerAppendToSBProc)(void* in_self, void* in_sourcebuffer, const unsigned char* in_data, unsigned in_length, const WKCMediaPlayerSourceBufferCallbacks* in_callbacks, void* in_opaque);
typedef int (*wkcMediaPlayerAbortSBProc)(void* in_self, void* in_sourcebuffer);
typedef bool (*wkcMediaPlayerSetSBTimestampOffsetProc)(void* in_self, void* in_sourcebuffer, double in_timestamp_offset);
typedef int (*wkcMediaPlayerRemoveSBProc)(void* in_self, void* in_sourcebuffer);
typedef int (*wkcMediaPlayerRemoveCodedFramesProc)(void* in_self, void* in_sourcebuffer, double in_start, double in_end);
typedef int (*wkcMediaPlayerEvictCodedFramesSBProc)(void* in_self, void* in_sourcebuffer, double* out_start, double* out_end);
typedef bool (*wkcMediaPlayerIsFullSBProc)(void* in_self, void* in_sourcebuffer);

struct WKCMediaPlayerProcs_ {
    wkcMediaPlayerInitializeProc fInitializeProc;
    wkcMediaPlayerFinalizeProc fFinalizeProc;
    wkcMediaPlayerForceTerminateProc fForceTerminateProc;
    wkcMediaPlayerIsSupportedMIMETypeProc fIsSupportedMIMETypeProc;
    wkcMediaPlayerSupportedMIMETypesProc fSupportedMIMETypesProc;
    wkcMediaPlayerSupportedMIMETypeProc fSupportedMIMETypeProc;
    wkcMediaPlayerGetUIStringProc fGetUIStringProc;
    wkcMediaPlayerGetUIStringTimeProc fGetUIStringTimeProc;
    wkcMediaPlayerCreateProc fCreateProc;
    wkcMediaPlayerDeleteProc fDeleteProc;
    wkcMediaPlayerLoadProc fLoadProc;
    wkcMediaPlayerCancelLoadProc fCancelLoadProc;
    wkcMediaPlayerPrepareToPlayProc fPrepareToPlayProc;
    wkcMediaPlayerPlayProc fPlayProc;
    wkcMediaPlayerPauseProc fPauseProc;
    wkcMediaPlayerIsPausedProc fIsPausedProc;
    wkcMediaPlayerHasVideoProc fHasVideoProc;
    wkcMediaPlayerHasAudioProc fHasAudioProc;
    wkcMediaPlayerSupportsFullScreenProc fSupportsFullScreenProc;
    wkcMediaPlayerCanSaveMediaDataProc fCanSaveMediaDataProc;
    wkcMediaPlayerHasClosedCaptionsProc fHasClosedCaptionsProc;
    wkcMediaPlayerSetClosedCaptionsVisibleProc fSetClosedCaptionsVisibleProc;
    wkcMediaPlayerHasSingleSecurityOriginProc fHasSingleSecurityOriginProc;
    wkcMediaPlayerNaturalSizeProc fNaturalSizeProc;
    wkcMediaPlayerSetSizeProc fSetSizeProc;
    wkcMediaPlayerSetVisibleProc fSetVisibleProc;
    wkcMediaPlayerDurationProc fDurationProc;
    wkcMediaPlayerStartTimeProc fStartTimeProc;
    wkcMediaPlayerCurrentTimeProc fCurrentTimeProc;
    wkcMediaPlayerMaxTimeSeekableProc fMaxTimeSeekableProc;
    wkcMediaPlayerSeekProc fSeekProc;
    wkcMediaPlayerRateProc fRateProc;
    wkcMediaPlayerSetRateProc fSetRateProc;
    wkcMediaPlayerSetPreservesPitchProc fSetPreservesPitchProc;
    wkcMediaPlayerVolumeProc fVolumeProc;
    wkcMediaPlayerSetVolumeProc fSetVolumeProc;
    wkcMediaPlayerNetworkStateProc fNetworkStateProc;
    wkcMediaPlayerReadyStateProc fReadyStateProc;
    wkcMediaPlayerMovieLoadTypeProc fMovieLoadTypeProc;
    wkcMediaPlayerSetWindowPositionProc fSetWindowPositionProc;
    wkcMediaPlayerVideoSinkTypeProc fVideoSinkTypeProc;
    wkcMediaPlayerBufferedRangesProc fBufferedRangesProc;
    wkcMediaPlayerBufferedRangeProc fBufferedRangeProc;
    wkcMediaPlayerRemovedFromDocumentProc fRemovedFromDocumentProc;
    wkcMediaPlayerWillBecomeInactiveProc fWillBecomeInactiveProc;
    wkcMediaPlayerEnterFullscreenProc fEnterFullscreenProc;
    wkcMediaPlayerExitFullscreenProc fExitFullscreenProc;
    wkcMediaPlayerCanEnterFullscreenProc fCanEnterFullscreenProc;
    wkcMediaPlayerDecodeAudioDataProc fDecodeAudioDataProc;
    wkcMediaPlayerAddSBProc fAddSBProc;
    wkcMediaPlayerGetMSDurationProc fGetMSDurationProc;
    wkcMediaPlayerSetMSDurationProc fSetMSDurationProc;
    wkcMediaPlayerNotifyMSMarkEndOfStreamProc fNotifyMSMarkEndOfStreamProc;
    wkcMediaPlayerNotifyMSUnmarkEndOfStreamProc fNotifyMSUnmarkEndOfStreamProc;
    wkcMediaPlayerSetReadyStateProc fSetReadyStateProc;
    wkcMediaPlayerAppendToSBProc fAppendToSBProc;
    wkcMediaPlayerAbortSBProc fAbortSBProc;
    wkcMediaPlayerSetSBTimestampOffsetProc fSetSBTimestampOffsetProc;
    wkcMediaPlayerRemoveSBProc fRemoveSBProc;
    wkcMediaPlayerRemoveCodedFramesProc fRemoveCodedFramesProc;
    wkcMediaPlayerEvictCodedFramesSBProc fEvictCodedFramesSBProc;
    wkcMediaPlayerIsFullSBProc fIsFullSBProc;
};
typedef struct WKCMediaPlayerProcs_ WKCMediaPlayerProcs;

// https://github.com/syvb/switch-oss/blob/master/WKC_2.26.0/include/wkc/wkcthreadprocs.h

typedef bool (*wkcWillCreateThreadProc)(const void* in_thread_id, const char* in_name, int* inout_priority, int* inout_core_number, unsigned int* inout_stack_size);
typedef void (*wkcDidCreateThreadProc)(const void* in_thread_id, void* in_native_thread_handle, const char* in_name, int in_priority, int in_core_number, unsigned int in_stack_size);
typedef void (*wkcDidDestroyThreadProc)(const void* in_thread_id, void* in_native_thread_handle);
typedef void* (*wkcAllocThreadStackProc)(unsigned int in_stack_size);
typedef void (*wkcFreeThreadStackProc)(void* in_stack_address);

struct WKCThreadProcs_ {
    wkcWillCreateThreadProc fWillCreateThreadProc;
    wkcDidCreateThreadProc  fDidCreateThreadProc;
    wkcDidDestroyThreadProc fDidDestroyThreadProc;
    wkcAllocThreadStackProc fAllocThreadStackProc;
    wkcFreeThreadStackProc fFreeThreadStackProc;
};
typedef struct WKCThreadProcs_ WKCThreadProcs;
