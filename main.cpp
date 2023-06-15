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
    aml->Hook(dlsym(pGTASA, "alAuxiliaryEffectSlotf"), (void*)alAuxiliaryEffectSlotf);
    aml->Hook(dlsym(pGTASA, "alAuxiliaryEffectSlotfv"), (void*)alAuxiliaryEffectSlotfv);
    aml->Hook(dlsym(pGTASA, "alAuxiliaryEffectSloti"), (void*)alAuxiliaryEffectSloti);
    aml->Hook(dlsym(pGTASA, "alAuxiliaryEffectSlotiv"), (void*)alAuxiliaryEffectSlotiv);
    aml->Hook(dlsym(pGTASA, "alBuffer3f"), (void*)alBuffer3f);
    aml->Hook(dlsym(pGTASA, "alBuffer3i"), (void*)alBuffer3i);
    aml->Hook(dlsym(pGTASA, "alBufferData"), (void*)alBufferData);
    aml->Hook(dlsym(pGTASA, "alBufferSamplesSOFT"), (void*)alBufferSamplesSOFT);
    aml->Hook(dlsym(pGTASA, "alBufferSubDataSOFT"), (void*)alBufferSubDataSOFT);
    aml->Hook(dlsym(pGTASA, "alBufferSubSamplesSOFT"), (void*)alBufferSubSamplesSOFT);
    aml->Hook(dlsym(pGTASA, "alBufferf"), (void*)alBufferf);
    aml->Hook(dlsym(pGTASA, "alBufferfv"), (void*)alBufferfv);
    aml->Hook(dlsym(pGTASA, "alBufferi"), (void*)alBufferi);
    //aml->Hook(dlsym(pGTASA, "alBufferiv"), (void*)alBufferiv); // crash? SEGV_ACCERR, not used anyway so i dont care...
    aml->Hook(dlsym(pGTASA, "alDeferUpdatesSOFT"), (void*)alDeferUpdatesSOFT);
    aml->Hook(dlsym(pGTASA, "alDeleteAuxiliaryEffectSlots"), (void*)alDeleteAuxiliaryEffectSlots);
    aml->Hook(dlsym(pGTASA, "alDeleteBuffers"), (void*)alDeleteBuffers);
    aml->Hook(dlsym(pGTASA, "alDeleteEffects"), (void*)alDeleteEffects);
    aml->Hook(dlsym(pGTASA, "alDeleteFilters"), (void*)alDeleteFilters);
    aml->Hook(dlsym(pGTASA, "alDeleteSources"), (void*)alDeleteSources);
    aml->Hook(dlsym(pGTASA, "alDisable"), (void*)alDisable);
    aml->Hook(dlsym(pGTASA, "alDistanceModel"), (void*)alDistanceModel);
    aml->Hook(dlsym(pGTASA, "alDopplerFactor"), (void*)alDopplerFactor);
    aml->Hook(dlsym(pGTASA, "alDopplerVelocity"), (void*)alDopplerVelocity);
    aml->Hook(dlsym(pGTASA, "alEffectf"), (void*)alEffectf);
    aml->Hook(dlsym(pGTASA, "alEffectfv"), (void*)alEffectfv);
    aml->Hook(dlsym(pGTASA, "alEffecti"), (void*)alEffecti);
    aml->Hook(dlsym(pGTASA, "alEffectiv"), (void*)alEffectiv);
    aml->Hook(dlsym(pGTASA, "alEnable"), (void*)alEnable);
    aml->Hook(dlsym(pGTASA, "alFilterf"), (void*)alFilterf);
    aml->Hook(dlsym(pGTASA, "alFilterfv"), (void*)alFilterfv);
    aml->Hook(dlsym(pGTASA, "alFilteri"), (void*)alFilteri);
    aml->Hook(dlsym(pGTASA, "alFilteriv"), (void*)alFilteriv);
    aml->Hook(dlsym(pGTASA, "alGenBuffers"), (void*)alGenBuffers);
    aml->Hook(dlsym(pGTASA, "alGenEffects"), (void*)alGenEffects);
    aml->Hook(dlsym(pGTASA, "alGenFilters"), (void*)alGenFilters);
    aml->Hook(dlsym(pGTASA, "alGenSources"), (void*)alGenSources);
    aml->Hook(dlsym(pGTASA, "alGetAuxiliaryEffectSlotf"), (void*)alGetAuxiliaryEffectSlotf);
    aml->Hook(dlsym(pGTASA, "alGetAuxiliaryEffectSlotfv"), (void*)alGetAuxiliaryEffectSlotfv);
    aml->Hook(dlsym(pGTASA, "alGetAuxiliaryEffectSloti"), (void*)alGetAuxiliaryEffectSloti);
    aml->Hook(dlsym(pGTASA, "alGetAuxiliaryEffectSlotiv"), (void*)alGetAuxiliaryEffectSlotiv);
    aml->Hook(dlsym(pGTASA, "alGetBoolean"), (void*)alGetBoolean);
    aml->Hook(dlsym(pGTASA, "alGetBooleanv"), (void*)alGetBooleanv);
    aml->Hook(dlsym(pGTASA, "alGetBuffer3f"), (void*)alGetBuffer3f);
    aml->Hook(dlsym(pGTASA, "alGetBuffer3i"), (void*)alGetBuffer3i);
    aml->Hook(dlsym(pGTASA, "alGetBufferSamplesSOFT"), (void*)alGetBufferSamplesSOFT);
    aml->Hook(dlsym(pGTASA, "alGetBufferf"), (void*)alGetBufferf);
    aml->Hook(dlsym(pGTASA, "alGetBufferfv"), (void*)alGetBufferfv);
    aml->Hook(dlsym(pGTASA, "alGetBufferi"), (void*)alGetBufferi);
    aml->Hook(dlsym(pGTASA, "alGetBufferiv"), (void*)alGetBufferiv);
    aml->Hook(dlsym(pGTASA, "alGetDouble"), (void*)alGetDouble);
    aml->Hook(dlsym(pGTASA, "alGetDoublev"), (void*)alGetDoublev);
    aml->Hook(dlsym(pGTASA, "alGetEffectf"), (void*)alGetEffectf);
    aml->Hook(dlsym(pGTASA, "alGetEffectfv"), (void*)alGetEffectfv);
    aml->Hook(dlsym(pGTASA, "alGetEffecti"), (void*)alGetEffecti);
    aml->Hook(dlsym(pGTASA, "alGetEffectiv"), (void*)alGetEffectiv);
    aml->Hook(dlsym(pGTASA, "alGetEnumValue"), (void*)alGetEnumValue);
    aml->Hook(dlsym(pGTASA, "alGetError"), (void*)alGetError);
    aml->Hook(dlsym(pGTASA, "alGetFilterf"), (void*)alGetFilterf);
    aml->Hook(dlsym(pGTASA, "alGetFilterfv"), (void*)alGetFilterfv);
    aml->Hook(dlsym(pGTASA, "alGetFilteri"), (void*)alGetFilteri);
    aml->Hook(dlsym(pGTASA, "alGetFilteriv"), (void*)alGetFilteriv);
    aml->Hook(dlsym(pGTASA, "alGetFloat"), (void*)alGetFloat);
    aml->Hook(dlsym(pGTASA, "alGetFloatv"), (void*)alGetFloatv);
    aml->Hook(dlsym(pGTASA, "alGetInteger"), (void*)alGetInteger);
    aml->Hook(dlsym(pGTASA, "alGetIntegerv"), (void*)alGetIntegerv);
    aml->Hook(dlsym(pGTASA, "alGetListener3f"), (void*)alGetListener3f);
    aml->Hook(dlsym(pGTASA, "alGetListener3i"), (void*)alGetListener3i);
    aml->Hook(dlsym(pGTASA, "alGetListenerf"), (void*)alGetListenerf);
    aml->Hook(dlsym(pGTASA, "alGetListenerfv"), (void*)alGetListenerfv);
    aml->Hook(dlsym(pGTASA, "alGetListeneri"), (void*)alGetListeneri);
    aml->Hook(dlsym(pGTASA, "alGetListeneriv"), (void*)alGetListeneriv);
    aml->Hook(dlsym(pGTASA, "alGetProcAddress"), (void*)alGetProcAddress);
    aml->Hook(dlsym(pGTASA, "alGetSource3dSOFT"), (void*)alGetSource3dSOFT);
    aml->Hook(dlsym(pGTASA, "alGetSource3f"), (void*)alGetSource3f);
    aml->Hook(dlsym(pGTASA, "alGetSource3i"), (void*)alGetSource3i);
    aml->Hook(dlsym(pGTASA, "alGetSource3i64SOFT"), (void*)alGetSource3i64SOFT);
    aml->Hook(dlsym(pGTASA, "alGetSourcedSOFT"), (void*)alGetSourcedSOFT);
    aml->Hook(dlsym(pGTASA, "alGetSourcedvSOFT"), (void*)alGetSourcedvSOFT);
    aml->Hook(dlsym(pGTASA, "alGetSourcef"), (void*)alGetSourcef);
    aml->Hook(dlsym(pGTASA, "alGetSourcefv"), (void*)alGetSourcefv);
    aml->Hook(dlsym(pGTASA, "alGetSourcei"), (void*)alGetSourcei);
    aml->Hook(dlsym(pGTASA, "alGetSourcei64SOFT"), (void*)alGetSourcei64SOFT);
    aml->Hook(dlsym(pGTASA, "alGetSourcei64vSOFT"), (void*)alGetSourcei64vSOFT);
    aml->Hook(dlsym(pGTASA, "alGetSourceiv"), (void*)alGetSourceiv);
    aml->Hook(dlsym(pGTASA, "alGetString"), (void*)alGetString);
    aml->Hook(dlsym(pGTASA, "alIsAuxiliaryEffectSlot"), (void*)alIsAuxiliaryEffectSlot);
    aml->Hook(dlsym(pGTASA, "alIsBuffer"), (void*)alIsBuffer);
    aml->Hook(dlsym(pGTASA, "alIsBufferFormatSupportedSOFT"), (void*)alIsBufferFormatSupportedSOFT);
    aml->Hook(dlsym(pGTASA, "alIsEffect"), (void*)alIsEffect);
    aml->Hook(dlsym(pGTASA, "alIsEnabled"), (void*)alIsEnabled);
    aml->Hook(dlsym(pGTASA, "alIsExtensionPresent"), (void*)alIsExtensionPresent);
    aml->Hook(dlsym(pGTASA, "alIsFilter"), (void*)alIsFilter);
    aml->Hook(dlsym(pGTASA, "alIsSource"), (void*)alIsSource);
    aml->Hook(dlsym(pGTASA, "alListener3f"), (void*)alListener3f);
    aml->Hook(dlsym(pGTASA, "alListener3i"), (void*)alListener3i);
    aml->Hook(dlsym(pGTASA, "alListenerf"), (void*)alListenerf);
    aml->Hook(dlsym(pGTASA, "alListenerfv"), (void*)alListenerfv);
    aml->Hook(dlsym(pGTASA, "alListeneri"), (void*)alListeneri);
    aml->Hook(dlsym(pGTASA, "alListeneriv"), (void*)alListeneriv);
    aml->Hook(dlsym(pGTASA, "alProcessUpdatesSOFT"), (void*)alProcessUpdatesSOFT);
    aml->Hook(dlsym(pGTASA, "alSource3dSOFT"), (void*)alSource3dSOFT);
    aml->Hook(dlsym(pGTASA, "alSource3f"), (void*)alSource3f);
    aml->Hook(dlsym(pGTASA, "alSource3i"), (void*)alSource3i);
    aml->Hook(dlsym(pGTASA, "alSource3i64SOFT"), (void*)alSource3i64SOFT);
    aml->Hook(dlsym(pGTASA, "alSourcePause"), (void*)alSourcePause);
    aml->Hook(dlsym(pGTASA, "alSourcePausev"), (void*)alSourcePausev);
    aml->Hook(dlsym(pGTASA, "alSourcePlay"), (void*)alSourcePlay);
    aml->Hook(dlsym(pGTASA, "alSourcePlayv"), (void*)alSourcePlayv);
    aml->Hook(dlsym(pGTASA, "alSourceQueueBuffers"), (void*)alSourceQueueBuffers);
    aml->Hook(dlsym(pGTASA, "alSourceRewind"), (void*)alSourceRewind);
    aml->Hook(dlsym(pGTASA, "alSourceRewindv"), (void*)alSourceRewindv);
    aml->Hook(dlsym(pGTASA, "alSourceStop"), (void*)alSourceStop);
    aml->Hook(dlsym(pGTASA, "alSourceStopv"), (void*)alSourceStopv);
    aml->Hook(dlsym(pGTASA, "alSourceUnqueueBuffers"), (void*)alSourceUnqueueBuffers);
    aml->Hook(dlsym(pGTASA, "alSourcedSOFT"), (void*)alSourcedSOFT);
    aml->Hook(dlsym(pGTASA, "alSourcedvSOFT"), (void*)alSourcedvSOFT);
    aml->Hook(dlsym(pGTASA, "alSourcef"), (void*)alSourcef);
    aml->Hook(dlsym(pGTASA, "alSourcefv"), (void*)alSourcefv);
    aml->Hook(dlsym(pGTASA, "alSourcei"), (void*)alSourcei);
    aml->Hook(dlsym(pGTASA, "alSourcei64SOFT"), (void*)alSourcei64SOFT);
    aml->Hook(dlsym(pGTASA, "alSourcei64vSOFT"), (void*)alSourcei64vSOFT);
    aml->Hook(dlsym(pGTASA, "alSourceiv"), (void*)alSourceiv);
    aml->Hook(dlsym(pGTASA, "alSpeedOfSound"), (void*)alSpeedOfSound);
    aml->Hook(dlsym(pGTASA, "alcCaptureCloseDevice"), (void*)alcCaptureCloseDevice);
    aml->Hook(dlsym(pGTASA, "alcCaptureOpenDevice"), (void*)alcCaptureOpenDevice);
    aml->Hook(dlsym(pGTASA, "alcCaptureSamples"), (void*)alcCaptureSamples);
    aml->Hook(dlsym(pGTASA, "alcCaptureStart"), (void*)alcCaptureStart);
    aml->Hook(dlsym(pGTASA, "alcCaptureStop"), (void*)alcCaptureStop);
    aml->Hook(dlsym(pGTASA, "alcCloseDevice"), (void*)alcCloseDevice);
    aml->Hook(dlsym(pGTASA, "alcCreateContext"), (void*)alcCreateContextHook);
    aml->Hook(dlsym(pGTASA, "alcDestroyContext"), (void*)alcDestroyContext);
    aml->Hook(dlsym(pGTASA, "alcGetContextsDevice"), (void*)alcGetContextsDevice);
    aml->Hook(dlsym(pGTASA, "alcGetCurrentContext"), (void*)alcGetCurrentContext);
    aml->Hook(dlsym(pGTASA, "alcGetEnumValue"), (void*)alcGetEnumValue);
    aml->Hook(dlsym(pGTASA, "alcGetError"), (void*)alcGetError);
    aml->Hook(dlsym(pGTASA, "alcGetIntegerv"), (void*)alcGetIntegerv);
    aml->Hook(dlsym(pGTASA, "alcGetProcAddress"), (void*)alcGetProcAddress);
    aml->Hook(dlsym(pGTASA, "alcGetString"), (void*)alcGetString);
    aml->Hook(dlsym(pGTASA, "alcGetThreadContext"), (void*)alcGetThreadContext);
    aml->Hook(dlsym(pGTASA, "alcIsExtensionPresent"), (void*)alcIsExtensionPresent);
    aml->Hook(dlsym(pGTASA, "alcIsRenderFormatSupportedSOFT"), (void*)alcIsRenderFormatSupportedSOFT);
    aml->Hook(dlsym(pGTASA, "alcLoopbackOpenDeviceSOFT"), (void*)alcLoopbackOpenDeviceSOFT);
    aml->Hook(dlsym(pGTASA, "alcMakeContextCurrent"), (void*)alcMakeContextCurrent);
    aml->Hook(dlsym(pGTASA, "alcOpenDevice"), (void*)alcOpenDevice);
    aml->Hook(dlsym(pGTASA, "alcProcessContext"), (void*)alcProcessContext);
    aml->Hook(dlsym(pGTASA, "alcRenderSamplesSOFT"), (void*)alcRenderSamplesSOFT);
    aml->Hook(dlsym(pGTASA, "alcSetThreadContext"), (void*)alcSetThreadContext);
    aml->Hook(dlsym(pGTASA, "alcSuspendContext"), (void*)alcSuspendContext);

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
