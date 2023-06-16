#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#define sizeofA(__aVar)  ((int)(sizeof(__aVar)/sizeof(__aVar[0])))

#include <include/AL/al.h>
#include <include/AL/alc.h>
#include <include/AL/alext.h>
#include <include/AL/efx.h>

#include "isautils.h"
ISAUtils* sautils = nullptr;

MYMODCFG(net.rusjj.gtasa.openal, OpenAL Soft, 1.1, kcat & TheOfficialFloW & RusJJ)

void* pGTASA = nullptr;
uintptr_t pGTASAAddr = 0;
ALCdevice* pSoundDevice = nullptr;
ALCcontext* pDeviceContext = nullptr;
const char* pYesNo[] = 
{
    "FEM_OFF",
    "FEM_ON",
};
const char* pFreq[] = 
{
    "22050 Hz",
    "44100 Hz",
};

ConfigEntry* pCfg44100Frequency;
ConfigEntry* pCfgHRTF;

bool UpdateALDevice()
{
    const ALCint attr[] = {
        ALC_FREQUENCY, pCfg44100Frequency->GetBool()?44100:22050,
        ALC_HRTF_SOFT, pCfgHRTF->GetBool(),
        0
    };
    return alcResetDeviceSOFT(pSoundDevice, attr);
}
ALCcontext *alcCreateContextHook(ALCdevice* device, const ALCint* attributes)
{
    if(pSoundDevice != nullptr) logger->Error("alcCreateContext created once more and with a different ALC Device?");
    pSoundDevice = device;
    const ALCint attr[] = {
        ALC_FREQUENCY, pCfg44100Frequency->GetBool()?44100:22050,
        ALC_HRTF_SOFT, pCfgHRTF->GetBool(),
        0 // END OF THE LIST
    };
    pDeviceContext = alcCreateContext(device, attr);
    return pDeviceContext;
}
void AudioFrequencyToggled(int oldVal, int newVal, void* data)
{
    pCfg44100Frequency->SetInt(newVal);
    UpdateALDevice();
    cfg->Save();
}
void HRTFToggled(int oldVal, int newVal, void* data)
{
    pCfgHRTF->SetInt(newVal);
    UpdateALDevice();
    cfg->Save();
}

int ZeroReturn(void* args, ...) { return 0; }

bool bPaused = false;
void PauseOpenSLES(int probably_alc_device)
{
    if(!bPaused)
    {
        alcDevicePauseSOFT(pSoundDevice);
        bPaused = true;
    }
}

void ResumeOpenSLES()
{
    if(bPaused)
    {
        alcDeviceResumeSOFT(pSoundDevice);
        bPaused = false;
    }
}

DECL_HOOK(char*, StartUserPause, void* self)
{
    ResumeOpenSLES();
    return StartUserPause(self);
}

DECL_HOOK(char*, EndUserPause, void* self)
{
    ResumeOpenSLES();
    return EndUserPause(self);
}

extern "C" void OnModLoad()
{
    logger->SetTag("AML OpenAL Soft");
    pCfg44100Frequency = cfg->Bind("44100hz", true);
    pCfgHRTF = cfg->Bind("HRTF", false);

    pGTASA = aml->GetLibHandle("libGTASA.so"); // aml->GetLib(...) returns address only! We need a HANDLE for GetSym!
    pGTASAAddr = aml->GetLib("libGTASA.so");

    aml->Redirect(aml->GetSym(pGTASA, "alAuxiliaryEffectSlotf"), (uintptr_t)alAuxiliaryEffectSlotf);
    aml->Redirect(aml->GetSym(pGTASA, "alAuxiliaryEffectSlotfv"), (uintptr_t)alAuxiliaryEffectSlotfv);
    aml->Redirect(aml->GetSym(pGTASA, "alAuxiliaryEffectSloti"), (uintptr_t)alAuxiliaryEffectSloti);
    aml->Redirect(aml->GetSym(pGTASA, "alAuxiliaryEffectSlotiv"), (uintptr_t)alAuxiliaryEffectSlotiv);
    aml->Redirect(aml->GetSym(pGTASA, "alBuffer3f"), (uintptr_t)alBuffer3f);
    aml->Redirect(aml->GetSym(pGTASA, "alBuffer3i"), (uintptr_t)alBuffer3i);
    aml->Redirect(aml->GetSym(pGTASA, "alBufferData"), (uintptr_t)alBufferData);
    aml->Redirect(aml->GetSym(pGTASA, "alBufferSamplesSOFT"), (uintptr_t)alBufferSamplesSOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alBufferSubDataSOFT"), (uintptr_t)alBufferSubDataSOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alBufferSubSamplesSOFT"), (uintptr_t)alBufferSubSamplesSOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alBufferf"), (uintptr_t)alBufferf);
    aml->Redirect(aml->GetSym(pGTASA, "alBufferfv"), (uintptr_t)alBufferfv);
    aml->Redirect(aml->GetSym(pGTASA, "alBufferi"), (uintptr_t)alBufferi);
    aml->Redirect(aml->GetSym(pGTASA, "alBufferiv"), (uintptr_t)alBufferiv); // crash? SEGV_ACCERR, not used anyway so i dont care...
    aml->Redirect(aml->GetSym(pGTASA, "alDeferUpdatesSOFT"), (uintptr_t)alDeferUpdatesSOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alDeleteAuxiliaryEffectSlots"), (uintptr_t)alDeleteAuxiliaryEffectSlots);
    aml->Redirect(aml->GetSym(pGTASA, "alDeleteBuffers"), (uintptr_t)alDeleteBuffers);
    aml->Redirect(aml->GetSym(pGTASA, "alDeleteEffects"), (uintptr_t)alDeleteEffects);
    aml->Redirect(aml->GetSym(pGTASA, "alDeleteFilters"), (uintptr_t)alDeleteFilters);
    aml->Redirect(aml->GetSym(pGTASA, "alDeleteSources"), (uintptr_t)alDeleteSources);
    aml->Redirect(aml->GetSym(pGTASA, "alDisable"), (uintptr_t)alDisable);
    aml->Redirect(aml->GetSym(pGTASA, "alDistanceModel"), (uintptr_t)alDistanceModel);
    aml->Redirect(aml->GetSym(pGTASA, "alDopplerFactor"), (uintptr_t)alDopplerFactor);
    aml->Redirect(aml->GetSym(pGTASA, "alDopplerVelocity"), (uintptr_t)alDopplerVelocity);
    aml->Redirect(aml->GetSym(pGTASA, "alEffectf"), (uintptr_t)alEffectf);
    aml->Redirect(aml->GetSym(pGTASA, "alEffectfv"), (uintptr_t)alEffectfv);
    aml->Redirect(aml->GetSym(pGTASA, "alEffecti"), (uintptr_t)alEffecti);
    aml->Redirect(aml->GetSym(pGTASA, "alEffectiv"), (uintptr_t)alEffectiv);
    aml->Redirect(aml->GetSym(pGTASA, "alEnable"), (uintptr_t)alEnable);
    aml->Redirect(aml->GetSym(pGTASA, "alFilterf"), (uintptr_t)alFilterf);
    aml->Redirect(aml->GetSym(pGTASA, "alFilterfv"), (uintptr_t)alFilterfv);
    aml->Redirect(aml->GetSym(pGTASA, "alFilteri"), (uintptr_t)alFilteri);
    aml->Redirect(aml->GetSym(pGTASA, "alFilteriv"), (uintptr_t)alFilteriv);
    aml->Redirect(aml->GetSym(pGTASA, "alGenBuffers"), (uintptr_t)alGenBuffers);
    aml->Redirect(aml->GetSym(pGTASA, "alGenEffects"), (uintptr_t)alGenEffects);
    aml->Redirect(aml->GetSym(pGTASA, "alGenFilters"), (uintptr_t)alGenFilters);
    aml->Redirect(aml->GetSym(pGTASA, "alGenSources"), (uintptr_t)alGenSources);
    aml->Redirect(aml->GetSym(pGTASA, "alGetAuxiliaryEffectSlotf"), (uintptr_t)alGetAuxiliaryEffectSlotf);
    aml->Redirect(aml->GetSym(pGTASA, "alGetAuxiliaryEffectSlotfv"), (uintptr_t)alGetAuxiliaryEffectSlotfv);
    aml->Redirect(aml->GetSym(pGTASA, "alGetAuxiliaryEffectSloti"), (uintptr_t)alGetAuxiliaryEffectSloti);
    aml->Redirect(aml->GetSym(pGTASA, "alGetAuxiliaryEffectSlotiv"), (uintptr_t)alGetAuxiliaryEffectSlotiv);
    aml->Redirect(aml->GetSym(pGTASA, "alGetBoolean"), (uintptr_t)alGetBoolean);
    aml->Redirect(aml->GetSym(pGTASA, "alGetBooleanv"), (uintptr_t)alGetBooleanv);
    aml->Redirect(aml->GetSym(pGTASA, "alGetBuffer3f"), (uintptr_t)alGetBuffer3f);
    aml->Redirect(aml->GetSym(pGTASA, "alGetBuffer3i"), (uintptr_t)alGetBuffer3i);
    aml->Redirect(aml->GetSym(pGTASA, "alGetBufferSamplesSOFT"), (uintptr_t)alGetBufferSamplesSOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alGetBufferf"), (uintptr_t)alGetBufferf);
    aml->Redirect(aml->GetSym(pGTASA, "alGetBufferfv"), (uintptr_t)alGetBufferfv);
    aml->Redirect(aml->GetSym(pGTASA, "alGetBufferi"), (uintptr_t)alGetBufferi);
    aml->Redirect(aml->GetSym(pGTASA, "alGetBufferiv"), (uintptr_t)alGetBufferiv);
    aml->Redirect(aml->GetSym(pGTASA, "alGetDouble"), (uintptr_t)alGetDouble);
    aml->Redirect(aml->GetSym(pGTASA, "alGetDoublev"), (uintptr_t)alGetDoublev);
    aml->Redirect(aml->GetSym(pGTASA, "alGetEffectf"), (uintptr_t)alGetEffectf);
    aml->Redirect(aml->GetSym(pGTASA, "alGetEffectfv"), (uintptr_t)alGetEffectfv);
    aml->Redirect(aml->GetSym(pGTASA, "alGetEffecti"), (uintptr_t)alGetEffecti);
    aml->Redirect(aml->GetSym(pGTASA, "alGetEffectiv"), (uintptr_t)alGetEffectiv);
    aml->Redirect(aml->GetSym(pGTASA, "alGetEnumValue"), (uintptr_t)alGetEnumValue);
    aml->Redirect(aml->GetSym(pGTASA, "alGetError"), (uintptr_t)alGetError);
    aml->Redirect(aml->GetSym(pGTASA, "alGetFilterf"), (uintptr_t)alGetFilterf);
    aml->Redirect(aml->GetSym(pGTASA, "alGetFilterfv"), (uintptr_t)alGetFilterfv);
    aml->Redirect(aml->GetSym(pGTASA, "alGetFilteri"), (uintptr_t)alGetFilteri);
    aml->Redirect(aml->GetSym(pGTASA, "alGetFilteriv"), (uintptr_t)alGetFilteriv);
    aml->Redirect(aml->GetSym(pGTASA, "alGetFloat"), (uintptr_t)alGetFloat);
    aml->Redirect(aml->GetSym(pGTASA, "alGetFloatv"), (uintptr_t)alGetFloatv);
    aml->Redirect(aml->GetSym(pGTASA, "alGetInteger"), (uintptr_t)alGetInteger);
    aml->Redirect(aml->GetSym(pGTASA, "alGetIntegerv"), (uintptr_t)alGetIntegerv);
    aml->Redirect(aml->GetSym(pGTASA, "alGetListener3f"), (uintptr_t)alGetListener3f);
    aml->Redirect(aml->GetSym(pGTASA, "alGetListener3i"), (uintptr_t)alGetListener3i);
    aml->Redirect(aml->GetSym(pGTASA, "alGetListenerf"), (uintptr_t)alGetListenerf);
    aml->Redirect(aml->GetSym(pGTASA, "alGetListenerfv"), (uintptr_t)alGetListenerfv);
    aml->Redirect(aml->GetSym(pGTASA, "alGetListeneri"), (uintptr_t)alGetListeneri);
    aml->Redirect(aml->GetSym(pGTASA, "alGetListeneriv"), (uintptr_t)alGetListeneriv);
    aml->Redirect(aml->GetSym(pGTASA, "alGetProcAddress"), (uintptr_t)alGetProcAddress);
    aml->Redirect(aml->GetSym(pGTASA, "alGetSource3dSOFT"), (uintptr_t)alGetSource3dSOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alGetSource3f"), (uintptr_t)alGetSource3f);
    aml->Redirect(aml->GetSym(pGTASA, "alGetSource3i"), (uintptr_t)alGetSource3i);
    aml->Redirect(aml->GetSym(pGTASA, "alGetSource3i64SOFT"), (uintptr_t)alGetSource3i64SOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alGetSourcedSOFT"), (uintptr_t)alGetSourcedSOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alGetSourcedvSOFT"), (uintptr_t)alGetSourcedvSOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alGetSourcef"), (uintptr_t)alGetSourcef);
    aml->Redirect(aml->GetSym(pGTASA, "alGetSourcefv"), (uintptr_t)alGetSourcefv);
    aml->Redirect(aml->GetSym(pGTASA, "alGetSourcei"), (uintptr_t)alGetSourcei);
    aml->Redirect(aml->GetSym(pGTASA, "alGetSourcei64SOFT"), (uintptr_t)alGetSourcei64SOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alGetSourcei64vSOFT"), (uintptr_t)alGetSourcei64vSOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alGetSourceiv"), (uintptr_t)alGetSourceiv);
    aml->Redirect(aml->GetSym(pGTASA, "alGetString"), (uintptr_t)alGetString);
    aml->Redirect(aml->GetSym(pGTASA, "alIsAuxiliaryEffectSlot"), (uintptr_t)alIsAuxiliaryEffectSlot);
    aml->Redirect(aml->GetSym(pGTASA, "alIsBuffer"), (uintptr_t)alIsBuffer);
    aml->Redirect(aml->GetSym(pGTASA, "alIsBufferFormatSupportedSOFT"), (uintptr_t)alIsBufferFormatSupportedSOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alIsEffect"), (uintptr_t)alIsEffect);
    aml->Redirect(aml->GetSym(pGTASA, "alIsEnabled"), (uintptr_t)alIsEnabled);
    aml->Redirect(aml->GetSym(pGTASA, "alIsExtensionPresent"), (uintptr_t)alIsExtensionPresent);
    aml->Redirect(aml->GetSym(pGTASA, "alIsFilter"), (uintptr_t)alIsFilter);
    aml->Redirect(aml->GetSym(pGTASA, "alIsSource"), (uintptr_t)alIsSource);
    aml->Redirect(aml->GetSym(pGTASA, "alListener3f"), (uintptr_t)alListener3f);
    aml->Redirect(aml->GetSym(pGTASA, "alListener3i"), (uintptr_t)alListener3i);
    aml->Redirect(aml->GetSym(pGTASA, "alListenerf"), (uintptr_t)alListenerf);
    aml->Redirect(aml->GetSym(pGTASA, "alListenerfv"), (uintptr_t)alListenerfv);
    aml->Redirect(aml->GetSym(pGTASA, "alListeneri"), (uintptr_t)alListeneri);
    aml->Redirect(aml->GetSym(pGTASA, "alListeneriv"), (uintptr_t)alListeneriv);
    aml->Redirect(aml->GetSym(pGTASA, "alProcessUpdatesSOFT"), (uintptr_t)alProcessUpdatesSOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alSource3dSOFT"), (uintptr_t)alSource3dSOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alSource3f"), (uintptr_t)alSource3f);
    aml->Redirect(aml->GetSym(pGTASA, "alSource3i"), (uintptr_t)alSource3i);
    aml->Redirect(aml->GetSym(pGTASA, "alSource3i64SOFT"), (uintptr_t)alSource3i64SOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alSourcePause"), (uintptr_t)alSourcePause);
    aml->Redirect(aml->GetSym(pGTASA, "alSourcePausev"), (uintptr_t)alSourcePausev);
    aml->Redirect(aml->GetSym(pGTASA, "alSourcePlay"), (uintptr_t)alSourcePlay);
    aml->Redirect(aml->GetSym(pGTASA, "alSourcePlayv"), (uintptr_t)alSourcePlayv);
    aml->Redirect(aml->GetSym(pGTASA, "alSourceQueueBuffers"), (uintptr_t)alSourceQueueBuffers);
    aml->Redirect(aml->GetSym(pGTASA, "alSourceRewind"), (uintptr_t)alSourceRewind);
    aml->Redirect(aml->GetSym(pGTASA, "alSourceRewindv"), (uintptr_t)alSourceRewindv);
    aml->Redirect(aml->GetSym(pGTASA, "alSourceStop"), (uintptr_t)alSourceStop);
    aml->Redirect(aml->GetSym(pGTASA, "alSourceStopv"), (uintptr_t)alSourceStopv);
    aml->Redirect(aml->GetSym(pGTASA, "alSourceUnqueueBuffers"), (uintptr_t)alSourceUnqueueBuffers);
    aml->Redirect(aml->GetSym(pGTASA, "alSourcedSOFT"), (uintptr_t)alSourcedSOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alSourcedvSOFT"), (uintptr_t)alSourcedvSOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alSourcef"), (uintptr_t)alSourcef);
    aml->Redirect(aml->GetSym(pGTASA, "alSourcefv"), (uintptr_t)alSourcefv);
    aml->Redirect(aml->GetSym(pGTASA, "alSourcei"), (uintptr_t)alSourcei);
    aml->Redirect(aml->GetSym(pGTASA, "alSourcei64SOFT"), (uintptr_t)alSourcei64SOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alSourcei64vSOFT"), (uintptr_t)alSourcei64vSOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alSourceiv"), (uintptr_t)alSourceiv);
    aml->Redirect(aml->GetSym(pGTASA, "alSpeedOfSound"), (uintptr_t)alSpeedOfSound);
    aml->Redirect(aml->GetSym(pGTASA, "alcCaptureCloseDevice"), (uintptr_t)alcCaptureCloseDevice);
    aml->Redirect(aml->GetSym(pGTASA, "alcCaptureOpenDevice"), (uintptr_t)alcCaptureOpenDevice);
    aml->Redirect(aml->GetSym(pGTASA, "alcCaptureSamples"), (uintptr_t)alcCaptureSamples);
    aml->Redirect(aml->GetSym(pGTASA, "alcCaptureStart"), (uintptr_t)alcCaptureStart);
    aml->Redirect(aml->GetSym(pGTASA, "alcCaptureStop"), (uintptr_t)alcCaptureStop);
    aml->Redirect(aml->GetSym(pGTASA, "alcCloseDevice"), (uintptr_t)alcCloseDevice);
    aml->Redirect(aml->GetSym(pGTASA, "alcCreateContext"), (uintptr_t)alcCreateContextHook);
    aml->Redirect(aml->GetSym(pGTASA, "alcDestroyContext"), (uintptr_t)alcDestroyContext);
    aml->Redirect(aml->GetSym(pGTASA, "alcGetContextsDevice"), (uintptr_t)alcGetContextsDevice);
    aml->Redirect(aml->GetSym(pGTASA, "alcGetCurrentContext"), (uintptr_t)alcGetCurrentContext);
    aml->Redirect(aml->GetSym(pGTASA, "alcGetEnumValue"), (uintptr_t)alcGetEnumValue);
    aml->Redirect(aml->GetSym(pGTASA, "alcGetError"), (uintptr_t)alcGetError);
    aml->Redirect(aml->GetSym(pGTASA, "alcGetIntegerv"), (uintptr_t)alcGetIntegerv);
    aml->Redirect(aml->GetSym(pGTASA, "alcGetProcAddress"), (uintptr_t)alcGetProcAddress);
    aml->Redirect(aml->GetSym(pGTASA, "alcGetString"), (uintptr_t)alcGetString);
    aml->Redirect(aml->GetSym(pGTASA, "alcGetThreadContext"), (uintptr_t)alcGetThreadContext);
    aml->Redirect(aml->GetSym(pGTASA, "alcIsExtensionPresent"), (uintptr_t)alcIsExtensionPresent);
    aml->Redirect(aml->GetSym(pGTASA, "alcIsRenderFormatSupportedSOFT"), (uintptr_t)alcIsRenderFormatSupportedSOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alcLoopbackOpenDeviceSOFT"), (uintptr_t)alcLoopbackOpenDeviceSOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alcMakeContextCurrent"), (uintptr_t)alcMakeContextCurrent);
    aml->Redirect(aml->GetSym(pGTASA, "alcOpenDevice"), (uintptr_t)alcOpenDevice);
    aml->Redirect(aml->GetSym(pGTASA, "alcProcessContext"), (uintptr_t)alcProcessContext);
    aml->Redirect(aml->GetSym(pGTASA, "alcRenderSamplesSOFT"), (uintptr_t)alcRenderSamplesSOFT);
    aml->Redirect(aml->GetSym(pGTASA, "alcSetThreadContext"), (uintptr_t)alcSetThreadContext);
    aml->Redirect(aml->GetSym(pGTASA, "alcSuspendContext"), (uintptr_t)alcSuspendContext);

    // Something by WarDrum
    aml->Hook(dlsym(pGTASA, "alSetConfigMOB"), (void*)ZeroReturn);
    aml->Hook(dlsym(pGTASA, "PauseOpenSLES"), (void*)PauseOpenSLES);
    HOOK(StartUserPause, dlsym(pGTASA, "_ZN6CTimer14StartUserPauseEv"));
    HOOK(EndUserPause, dlsym(pGTASA, "_ZN6CTimer12EndUserPauseEv"));

    sautils = (ISAUtils*)GetInterface("SAUtils");
    if(sautils != nullptr)
    {
        sautils->AddClickableItem(SetType_Audio, "Max Audio Frequency", pCfg44100Frequency->GetBool(), 0, sizeofA(pFreq)-1, pFreq, AudioFrequencyToggled, NULL);
        sautils->AddClickableItem(SetType_Audio, "HRTF", pCfgHRTF->GetBool(), 0, sizeofA(pYesNo)-1, pYesNo, HRTFToggled, NULL);
    }
}
