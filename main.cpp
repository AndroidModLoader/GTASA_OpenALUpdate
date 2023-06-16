#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#define sizeofA(__aVar)  ((int)(sizeof(__aVar)/sizeof(__aVar[0])))

#include <dlfcn.h>
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

    pGTASA = dlopen("libGTASA.so", RTLD_LAZY); // aml->GetLib(...) returns address only! We need a HANDLE for GetSym!
    pGTASAAddr = aml->GetLib("libGTASA.so");
    aml->Redirect(dlsym(pGTASA, "alAuxiliaryEffectSlotf"), (uintptr_t)alAuxiliaryEffectSlotf);
    aml->Redirect(dlsym(pGTASA, "alAuxiliaryEffectSlotfv"), (uintptr_t)alAuxiliaryEffectSlotfv);
    aml->Redirect(dlsym(pGTASA, "alAuxiliaryEffectSloti"), (uintptr_t)alAuxiliaryEffectSloti);
    aml->Redirect(dlsym(pGTASA, "alAuxiliaryEffectSlotiv"), (uintptr_t)alAuxiliaryEffectSlotiv);
    aml->Redirect(dlsym(pGTASA, "alBuffer3f"), (uintptr_t)alBuffer3f);
    aml->Redirect(dlsym(pGTASA, "alBuffer3i"), (uintptr_t)alBuffer3i);
    aml->Redirect(dlsym(pGTASA, "alBufferData"), (uintptr_t)alBufferData);
    aml->Redirect(dlsym(pGTASA, "alBufferSamplesSOFT"), (uintptr_t)alBufferSamplesSOFT);
    aml->Redirect(dlsym(pGTASA, "alBufferSubDataSOFT"), (uintptr_t)alBufferSubDataSOFT);
    aml->Redirect(dlsym(pGTASA, "alBufferSubSamplesSOFT"), (uintptr_t)alBufferSubSamplesSOFT);
    aml->Redirect(dlsym(pGTASA, "alBufferf"), (uintptr_t)alBufferf);
    aml->Redirect(dlsym(pGTASA, "alBufferfv"), (uintptr_t)alBufferfv);
    aml->Redirect(dlsym(pGTASA, "alBufferi"), (uintptr_t)alBufferi);
    aml->Redirect(dlsym(pGTASA, "alBufferiv"), (uintptr_t)alBufferiv); // crash? SEGV_ACCERR, not used anyway so i dont care...
    aml->Redirect(dlsym(pGTASA, "alDeferUpdatesSOFT"), (uintptr_t)alDeferUpdatesSOFT);
    aml->Redirect(dlsym(pGTASA, "alDeleteAuxiliaryEffectSlots"), (uintptr_t)alDeleteAuxiliaryEffectSlots);
    aml->Redirect(dlsym(pGTASA, "alDeleteBuffers"), (uintptr_t)alDeleteBuffers);
    aml->Redirect(dlsym(pGTASA, "alDeleteEffects"), (uintptr_t)alDeleteEffects);
    aml->Redirect(dlsym(pGTASA, "alDeleteFilters"), (uintptr_t)alDeleteFilters);
    aml->Redirect(dlsym(pGTASA, "alDeleteSources"), (uintptr_t)alDeleteSources);
    aml->Redirect(dlsym(pGTASA, "alDisable"), (uintptr_t)alDisable);
    aml->Redirect(dlsym(pGTASA, "alDistanceModel"), (uintptr_t)alDistanceModel);
    aml->Redirect(dlsym(pGTASA, "alDopplerFactor"), (uintptr_t)alDopplerFactor);
    aml->Redirect(dlsym(pGTASA, "alDopplerVelocity"), (uintptr_t)alDopplerVelocity);
    aml->Redirect(dlsym(pGTASA, "alEffectf"), (uintptr_t)alEffectf);
    aml->Redirect(dlsym(pGTASA, "alEffectfv"), (uintptr_t)alEffectfv);
    aml->Redirect(dlsym(pGTASA, "alEffecti"), (uintptr_t)alEffecti);
    aml->Redirect(dlsym(pGTASA, "alEffectiv"), (uintptr_t)alEffectiv);
    aml->Redirect(dlsym(pGTASA, "alEnable"), (uintptr_t)alEnable);
    aml->Redirect(dlsym(pGTASA, "alFilterf"), (uintptr_t)alFilterf);
    aml->Redirect(dlsym(pGTASA, "alFilterfv"), (uintptr_t)alFilterfv);
    aml->Redirect(dlsym(pGTASA, "alFilteri"), (uintptr_t)alFilteri);
    aml->Redirect(dlsym(pGTASA, "alFilteriv"), (uintptr_t)alFilteriv);
    aml->Redirect(dlsym(pGTASA, "alGenBuffers"), (uintptr_t)alGenBuffers);
    aml->Redirect(dlsym(pGTASA, "alGenEffects"), (uintptr_t)alGenEffects);
    aml->Redirect(dlsym(pGTASA, "alGenFilters"), (uintptr_t)alGenFilters);
    aml->Redirect(dlsym(pGTASA, "alGenSources"), (uintptr_t)alGenSources);
    aml->Redirect(dlsym(pGTASA, "alGetAuxiliaryEffectSlotf"), (uintptr_t)alGetAuxiliaryEffectSlotf);
    aml->Redirect(dlsym(pGTASA, "alGetAuxiliaryEffectSlotfv"), (uintptr_t)alGetAuxiliaryEffectSlotfv);
    aml->Redirect(dlsym(pGTASA, "alGetAuxiliaryEffectSloti"), (uintptr_t)alGetAuxiliaryEffectSloti);
    aml->Redirect(dlsym(pGTASA, "alGetAuxiliaryEffectSlotiv"), (uintptr_t)alGetAuxiliaryEffectSlotiv);
    aml->Redirect(dlsym(pGTASA, "alGetBoolean"), (uintptr_t)alGetBoolean);
    aml->Redirect(dlsym(pGTASA, "alGetBooleanv"), (uintptr_t)alGetBooleanv);
    aml->Redirect(dlsym(pGTASA, "alGetBuffer3f"), (uintptr_t)alGetBuffer3f);
    aml->Redirect(dlsym(pGTASA, "alGetBuffer3i"), (uintptr_t)alGetBuffer3i);
    aml->Redirect(dlsym(pGTASA, "alGetBufferSamplesSOFT"), (uintptr_t)alGetBufferSamplesSOFT);
    aml->Redirect(dlsym(pGTASA, "alGetBufferf"), (uintptr_t)alGetBufferf);
    aml->Redirect(dlsym(pGTASA, "alGetBufferfv"), (uintptr_t)alGetBufferfv);
    aml->Redirect(dlsym(pGTASA, "alGetBufferi"), (uintptr_t)alGetBufferi);
    aml->Redirect(dlsym(pGTASA, "alGetBufferiv"), (uintptr_t)alGetBufferiv);
    aml->Redirect(dlsym(pGTASA, "alGetDouble"), (uintptr_t)alGetDouble);
    aml->Redirect(dlsym(pGTASA, "alGetDoublev"), (uintptr_t)alGetDoublev);
    aml->Redirect(dlsym(pGTASA, "alGetEffectf"), (uintptr_t)alGetEffectf);
    aml->Redirect(dlsym(pGTASA, "alGetEffectfv"), (uintptr_t)alGetEffectfv);
    aml->Redirect(dlsym(pGTASA, "alGetEffecti"), (uintptr_t)alGetEffecti);
    aml->Redirect(dlsym(pGTASA, "alGetEffectiv"), (uintptr_t)alGetEffectiv);
    aml->Redirect(dlsym(pGTASA, "alGetEnumValue"), (uintptr_t)alGetEnumValue);
    aml->Redirect(dlsym(pGTASA, "alGetError"), (uintptr_t)alGetError);
    aml->Redirect(dlsym(pGTASA, "alGetFilterf"), (uintptr_t)alGetFilterf);
    aml->Redirect(dlsym(pGTASA, "alGetFilterfv"), (uintptr_t)alGetFilterfv);
    aml->Redirect(dlsym(pGTASA, "alGetFilteri"), (uintptr_t)alGetFilteri);
    aml->Redirect(dlsym(pGTASA, "alGetFilteriv"), (uintptr_t)alGetFilteriv);
    aml->Redirect(dlsym(pGTASA, "alGetFloat"), (uintptr_t)alGetFloat);
    aml->Redirect(dlsym(pGTASA, "alGetFloatv"), (uintptr_t)alGetFloatv);
    aml->Redirect(dlsym(pGTASA, "alGetInteger"), (uintptr_t)alGetInteger);
    aml->Redirect(dlsym(pGTASA, "alGetIntegerv"), (uintptr_t)alGetIntegerv);
    aml->Redirect(dlsym(pGTASA, "alGetListener3f"), (uintptr_t)alGetListener3f);
    aml->Redirect(dlsym(pGTASA, "alGetListener3i"), (uintptr_t)alGetListener3i);
    aml->Redirect(dlsym(pGTASA, "alGetListenerf"), (uintptr_t)alGetListenerf);
    aml->Redirect(dlsym(pGTASA, "alGetListenerfv"), (uintptr_t)alGetListenerfv);
    aml->Redirect(dlsym(pGTASA, "alGetListeneri"), (uintptr_t)alGetListeneri);
    aml->Redirect(dlsym(pGTASA, "alGetListeneriv"), (uintptr_t)alGetListeneriv);
    aml->Redirect(dlsym(pGTASA, "alGetProcAddress"), (uintptr_t)alGetProcAddress);
    aml->Redirect(dlsym(pGTASA, "alGetSource3dSOFT"), (uintptr_t)alGetSource3dSOFT);
    aml->Redirect(dlsym(pGTASA, "alGetSource3f"), (uintptr_t)alGetSource3f);
    aml->Redirect(dlsym(pGTASA, "alGetSource3i"), (uintptr_t)alGetSource3i);
    aml->Redirect(dlsym(pGTASA, "alGetSource3i64SOFT"), (uintptr_t)alGetSource3i64SOFT);
    aml->Redirect(dlsym(pGTASA, "alGetSourcedSOFT"), (uintptr_t)alGetSourcedSOFT);
    aml->Redirect(dlsym(pGTASA, "alGetSourcedvSOFT"), (uintptr_t)alGetSourcedvSOFT);
    aml->Redirect(dlsym(pGTASA, "alGetSourcef"), (uintptr_t)alGetSourcef);
    aml->Redirect(dlsym(pGTASA, "alGetSourcefv"), (uintptr_t)alGetSourcefv);
    aml->Redirect(dlsym(pGTASA, "alGetSourcei"), (uintptr_t)alGetSourcei);
    aml->Redirect(dlsym(pGTASA, "alGetSourcei64SOFT"), (uintptr_t)alGetSourcei64SOFT);
    aml->Redirect(dlsym(pGTASA, "alGetSourcei64vSOFT"), (uintptr_t)alGetSourcei64vSOFT);
    aml->Redirect(dlsym(pGTASA, "alGetSourceiv"), (uintptr_t)alGetSourceiv);
    aml->Redirect(dlsym(pGTASA, "alGetString"), (uintptr_t)alGetString);
    aml->Redirect(dlsym(pGTASA, "alIsAuxiliaryEffectSlot"), (uintptr_t)alIsAuxiliaryEffectSlot);
    aml->Redirect(dlsym(pGTASA, "alIsBuffer"), (uintptr_t)alIsBuffer);
    aml->Redirect(dlsym(pGTASA, "alIsBufferFormatSupportedSOFT"), (uintptr_t)alIsBufferFormatSupportedSOFT);
    aml->Redirect(dlsym(pGTASA, "alIsEffect"), (uintptr_t)alIsEffect);
    aml->Redirect(dlsym(pGTASA, "alIsEnabled"), (uintptr_t)alIsEnabled);
    aml->Redirect(dlsym(pGTASA, "alIsExtensionPresent"), (uintptr_t)alIsExtensionPresent);
    aml->Redirect(dlsym(pGTASA, "alIsFilter"), (uintptr_t)alIsFilter);
    aml->Redirect(dlsym(pGTASA, "alIsSource"), (uintptr_t)alIsSource);
    aml->Redirect(dlsym(pGTASA, "alListener3f"), (uintptr_t)alListener3f);
    aml->Redirect(dlsym(pGTASA, "alListener3i"), (uintptr_t)alListener3i);
    aml->Redirect(dlsym(pGTASA, "alListenerf"), (uintptr_t)alListenerf);
    aml->Redirect(dlsym(pGTASA, "alListenerfv"), (uintptr_t)alListenerfv);
    aml->Redirect(dlsym(pGTASA, "alListeneri"), (uintptr_t)alListeneri);
    aml->Redirect(dlsym(pGTASA, "alListeneriv"), (uintptr_t)alListeneriv);
    aml->Redirect(dlsym(pGTASA, "alProcessUpdatesSOFT"), (uintptr_t)alProcessUpdatesSOFT);
    aml->Redirect(dlsym(pGTASA, "alSource3dSOFT"), (uintptr_t)alSource3dSOFT);
    aml->Redirect(dlsym(pGTASA, "alSource3f"), (uintptr_t)alSource3f);
    aml->Redirect(dlsym(pGTASA, "alSource3i"), (uintptr_t)alSource3i);
    aml->Redirect(dlsym(pGTASA, "alSource3i64SOFT"), (uintptr_t)alSource3i64SOFT);
    aml->Redirect(dlsym(pGTASA, "alSourcePause"), (uintptr_t)alSourcePause);
    aml->Redirect(dlsym(pGTASA, "alSourcePausev"), (uintptr_t)alSourcePausev);
    aml->Redirect(dlsym(pGTASA, "alSourcePlay"), (uintptr_t)alSourcePlay);
    aml->Redirect(dlsym(pGTASA, "alSourcePlayv"), (uintptr_t)alSourcePlayv);
    aml->Redirect(dlsym(pGTASA, "alSourceQueueBuffers"), (uintptr_t)alSourceQueueBuffers);
    aml->Redirect(dlsym(pGTASA, "alSourceRewind"), (uintptr_t)alSourceRewind);
    aml->Redirect(dlsym(pGTASA, "alSourceRewindv"), (uintptr_t)alSourceRewindv);
    aml->Redirect(dlsym(pGTASA, "alSourceStop"), (uintptr_t)alSourceStop);
    aml->Redirect(dlsym(pGTASA, "alSourceStopv"), (uintptr_t)alSourceStopv);
    aml->Redirect(dlsym(pGTASA, "alSourceUnqueueBuffers"), (uintptr_t)alSourceUnqueueBuffers);
    aml->Redirect(dlsym(pGTASA, "alSourcedSOFT"), (uintptr_t)alSourcedSOFT);
    aml->Redirect(dlsym(pGTASA, "alSourcedvSOFT"), (uintptr_t)alSourcedvSOFT);
    aml->Redirect(dlsym(pGTASA, "alSourcef"), (uintptr_t)alSourcef);
    aml->Redirect(dlsym(pGTASA, "alSourcefv"), (uintptr_t)alSourcefv);
    aml->Redirect(dlsym(pGTASA, "alSourcei"), (uintptr_t)alSourcei);
    aml->Redirect(dlsym(pGTASA, "alSourcei64SOFT"), (uintptr_t)alSourcei64SOFT);
    aml->Redirect(dlsym(pGTASA, "alSourcei64vSOFT"), (uintptr_t)alSourcei64vSOFT);
    aml->Redirect(dlsym(pGTASA, "alSourceiv"), (uintptr_t)alSourceiv);
    aml->Redirect(dlsym(pGTASA, "alSpeedOfSound"), (uintptr_t)alSpeedOfSound);
    aml->Redirect(dlsym(pGTASA, "alcCaptureCloseDevice"), (uintptr_t)alcCaptureCloseDevice);
    aml->Redirect(dlsym(pGTASA, "alcCaptureOpenDevice"), (uintptr_t)alcCaptureOpenDevice);
    aml->Redirect(dlsym(pGTASA, "alcCaptureSamples"), (uintptr_t)alcCaptureSamples);
    aml->Redirect(dlsym(pGTASA, "alcCaptureStart"), (uintptr_t)alcCaptureStart);
    aml->Redirect(dlsym(pGTASA, "alcCaptureStop"), (uintptr_t)alcCaptureStop);
    aml->Redirect(dlsym(pGTASA, "alcCloseDevice"), (uintptr_t)alcCloseDevice);
    aml->Redirect(dlsym(pGTASA, "alcCreateContext"), (uintptr_t)alcCreateContextHook);
    aml->Redirect(dlsym(pGTASA, "alcDestroyContext"), (uintptr_t)alcDestroyContext);
    aml->Redirect(dlsym(pGTASA, "alcGetContextsDevice"), (uintptr_t)alcGetContextsDevice);
    aml->Redirect(dlsym(pGTASA, "alcGetCurrentContext"), (uintptr_t)alcGetCurrentContext);
    aml->Redirect(dlsym(pGTASA, "alcGetEnumValue"), (uintptr_t)alcGetEnumValue);
    aml->Redirect(dlsym(pGTASA, "alcGetError"), (uintptr_t)alcGetError);
    aml->Redirect(dlsym(pGTASA, "alcGetIntegerv"), (uintptr_t)alcGetIntegerv);
    aml->Redirect(dlsym(pGTASA, "alcGetProcAddress"), (uintptr_t)alcGetProcAddress);
    aml->Redirect(dlsym(pGTASA, "alcGetString"), (uintptr_t)alcGetString);
    aml->Redirect(dlsym(pGTASA, "alcGetThreadContext"), (uintptr_t)alcGetThreadContext);
    aml->Redirect(dlsym(pGTASA, "alcIsExtensionPresent"), (uintptr_t)alcIsExtensionPresent);
    aml->Redirect(dlsym(pGTASA, "alcIsRenderFormatSupportedSOFT"), (uintptr_t)alcIsRenderFormatSupportedSOFT);
    aml->Redirect(dlsym(pGTASA, "alcLoopbackOpenDeviceSOFT"), (uintptr_t)alcLoopbackOpenDeviceSOFT);
    aml->Redirect(dlsym(pGTASA, "alcMakeContextCurrent"), (uintptr_t)alcMakeContextCurrent);
    aml->Redirect(dlsym(pGTASA, "alcOpenDevice"), (uintptr_t)alcOpenDevice);
    aml->Redirect(dlsym(pGTASA, "alcProcessContext"), (uintptr_t)alcProcessContext);
    aml->Redirect(dlsym(pGTASA, "alcRenderSamplesSOFT"), (uintptr_t)alcRenderSamplesSOFT);
    aml->Redirect(dlsym(pGTASA, "alcSetThreadContext"), (uintptr_t)alcSetThreadContext);
    aml->Redirect(dlsym(pGTASA, "alcSuspendContext"), (uintptr_t)alcSuspendContext);

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
