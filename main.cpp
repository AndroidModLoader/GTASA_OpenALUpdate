#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#define sizeofA(__aVar)  ((int)(sizeof(__aVar)/sizeof(__aVar[0])))

#include <dlfcn.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>

#include "isautils.h"
ISAUtils* sautils = nullptr;

MYMODCFG(net.rusjj.gtasa.openal, OpenAL Soft, 1.0, kcat & TheOfficialFloW & RusJJ)

uintptr_t pGTASA = 0;
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
    pSoundDevice = device;
    const ALCint attr[] = {
        ALC_FREQUENCY, pCfg44100Frequency->GetBool()?44100:22050,
        ALC_HRTF_SOFT, pCfgHRTF->GetBool(),
        0 // END OF THE LIST
    };
    pDeviceContext = alcCreateContext(device, attr);
    return pDeviceContext;
}
void AudioFrequencyToggled(int oldVal, int newVal)
{
    pCfg44100Frequency->SetInt(newVal);
    UpdateALDevice();
    cfg->Save();
}
void HRTFToggled(int oldVal, int newVal)
{
    pCfgHRTF->SetInt(newVal);
    UpdateALDevice();
    cfg->Save();
}

int ZeroReturn() { return 0; }

extern "C" void OnModLoad()
{
    logger->SetTag("AML OpenAL Soft");
    pCfg44100Frequency = cfg->Bind("44100hz", true);
    pCfgHRTF = cfg->Bind("HRTF", false);

    pGTASA = (uintptr_t)dlopen("libGTASA.so", RTLD_LAZY); // aml->GetLib(...) returns address only! We need a HANDLE for GetSym!
    aml->Hook((void*)aml->GetSym(pGTASA, "alAuxiliaryEffectSlotf"), (void*)alAuxiliaryEffectSlotf);
    aml->Hook((void*)aml->GetSym(pGTASA, "alAuxiliaryEffectSlotfv"), (void*)alAuxiliaryEffectSlotfv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alAuxiliaryEffectSloti"), (void*)alAuxiliaryEffectSloti);
    aml->Hook((void*)aml->GetSym(pGTASA, "alAuxiliaryEffectSlotiv"), (void*)alAuxiliaryEffectSlotiv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alBuffer3f"), (void*)alBuffer3f);
    aml->Hook((void*)aml->GetSym(pGTASA, "alBuffer3i"), (void*)alBuffer3i);
    aml->Hook((void*)aml->GetSym(pGTASA, "alBufferData"), (void*)alBufferData);
    aml->Hook((void*)aml->GetSym(pGTASA, "alBufferSamplesSOFT"), (void*)alBufferSamplesSOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alBufferSubDataSOFT"), (void*)alBufferSubDataSOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alBufferSubSamplesSOFT"), (void*)alBufferSubSamplesSOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alBufferf"), (void*)alBufferf);
    aml->Hook((void*)aml->GetSym(pGTASA, "alBufferfv"), (void*)alBufferfv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alBufferi"), (void*)alBufferi);
    //aml->Hook((void*)aml->GetSym(pGTASA, "alBufferiv"), (void*)alBufferiv); // crash?
    aml->Hook((void*)aml->GetSym(pGTASA, "alDeferUpdatesSOFT"), (void*)alDeferUpdatesSOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alDeleteAuxiliaryEffectSlots"), (void*)alDeleteAuxiliaryEffectSlots);
    aml->Hook((void*)aml->GetSym(pGTASA, "alDeleteBuffers"), (void*)alDeleteBuffers);
    aml->Hook((void*)aml->GetSym(pGTASA, "alDeleteEffects"), (void*)alDeleteEffects);
    aml->Hook((void*)aml->GetSym(pGTASA, "alDeleteFilters"), (void*)alDeleteFilters);
    aml->Hook((void*)aml->GetSym(pGTASA, "alDeleteSources"), (void*)alDeleteSources);
    aml->Hook((void*)aml->GetSym(pGTASA, "alDisable"), (void*)alDisable);
    aml->Hook((void*)aml->GetSym(pGTASA, "alDistanceModel"), (void*)alDistanceModel);
    aml->Hook((void*)aml->GetSym(pGTASA, "alDopplerFactor"), (void*)alDopplerFactor);
    aml->Hook((void*)aml->GetSym(pGTASA, "alDopplerVelocity"), (void*)alDopplerVelocity);
    aml->Hook((void*)aml->GetSym(pGTASA, "alEffectf"), (void*)alEffectf);
    aml->Hook((void*)aml->GetSym(pGTASA, "alEffectfv"), (void*)alEffectfv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alEffecti"), (void*)alEffecti);
    aml->Hook((void*)aml->GetSym(pGTASA, "alEffectiv"), (void*)alEffectiv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alEnable"), (void*)alEnable);
    aml->Hook((void*)aml->GetSym(pGTASA, "alFilterf"), (void*)alFilterf);
    aml->Hook((void*)aml->GetSym(pGTASA, "alFilterfv"), (void*)alFilterfv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alFilteri"), (void*)alFilteri);
    aml->Hook((void*)aml->GetSym(pGTASA, "alFilteriv"), (void*)alFilteriv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGenBuffers"), (void*)alGenBuffers);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGenEffects"), (void*)alGenEffects);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGenFilters"), (void*)alGenFilters);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGenSources"), (void*)alGenSources);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetAuxiliaryEffectSlotf"), (void*)alGetAuxiliaryEffectSlotf);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetAuxiliaryEffectSlotfv"), (void*)alGetAuxiliaryEffectSlotfv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetAuxiliaryEffectSloti"), (void*)alGetAuxiliaryEffectSloti);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetAuxiliaryEffectSlotiv"), (void*)alGetAuxiliaryEffectSlotiv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetBoolean"), (void*)alGetBoolean);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetBooleanv"), (void*)alGetBooleanv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetBuffer3f"), (void*)alGetBuffer3f);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetBuffer3i"), (void*)alGetBuffer3i);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetBufferSamplesSOFT"), (void*)alGetBufferSamplesSOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetBufferf"), (void*)alGetBufferf);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetBufferfv"), (void*)alGetBufferfv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetBufferi"), (void*)alGetBufferi);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetBufferiv"), (void*)alGetBufferiv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetDouble"), (void*)alGetDouble);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetDoublev"), (void*)alGetDoublev);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetEffectf"), (void*)alGetEffectf);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetEffectfv"), (void*)alGetEffectfv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetEffecti"), (void*)alGetEffecti);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetEffectiv"), (void*)alGetEffectiv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetEnumValue"), (void*)alGetEnumValue);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetError"), (void*)alGetError);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetFilterf"), (void*)alGetFilterf);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetFilterfv"), (void*)alGetFilterfv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetFilteri"), (void*)alGetFilteri);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetFilteriv"), (void*)alGetFilteriv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetFloat"), (void*)alGetFloat);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetFloatv"), (void*)alGetFloatv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetInteger"), (void*)alGetInteger);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetIntegerv"), (void*)alGetIntegerv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetListener3f"), (void*)alGetListener3f);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetListener3i"), (void*)alGetListener3i);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetListenerf"), (void*)alGetListenerf);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetListenerfv"), (void*)alGetListenerfv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetListeneri"), (void*)alGetListeneri);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetListeneriv"), (void*)alGetListeneriv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetProcAddress"), (void*)alGetProcAddress);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetSource3dSOFT"), (void*)alGetSource3dSOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetSource3f"), (void*)alGetSource3f);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetSource3i"), (void*)alGetSource3i);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetSource3i64SOFT"), (void*)alGetSource3i64SOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetSourcedSOFT"), (void*)alGetSourcedSOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetSourcedvSOFT"), (void*)alGetSourcedvSOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetSourcef"), (void*)alGetSourcef);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetSourcefv"), (void*)alGetSourcefv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetSourcei"), (void*)alGetSourcei);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetSourcei64SOFT"), (void*)alGetSourcei64SOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetSourcei64vSOFT"), (void*)alGetSourcei64vSOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetSourceiv"), (void*)alGetSourceiv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alGetString"), (void*)alGetString);
    aml->Hook((void*)aml->GetSym(pGTASA, "alIsAuxiliaryEffectSlot"), (void*)alIsAuxiliaryEffectSlot);
    aml->Hook((void*)aml->GetSym(pGTASA, "alIsBuffer"), (void*)alIsBuffer);
    aml->Hook((void*)aml->GetSym(pGTASA, "alIsBufferFormatSupportedSOFT"), (void*)alIsBufferFormatSupportedSOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alIsEffect"), (void*)alIsEffect);
    aml->Hook((void*)aml->GetSym(pGTASA, "alIsEnabled"), (void*)alIsEnabled);
    aml->Hook((void*)aml->GetSym(pGTASA, "alIsExtensionPresent"), (void*)alIsExtensionPresent);
    aml->Hook((void*)aml->GetSym(pGTASA, "alIsFilter"), (void*)alIsFilter);
    aml->Hook((void*)aml->GetSym(pGTASA, "alIsSource"), (void*)alIsSource);
    aml->Hook((void*)aml->GetSym(pGTASA, "alListener3f"), (void*)alListener3f);
    aml->Hook((void*)aml->GetSym(pGTASA, "alListener3i"), (void*)alListener3i);
    aml->Hook((void*)aml->GetSym(pGTASA, "alListenerf"), (void*)alListenerf);
    aml->Hook((void*)aml->GetSym(pGTASA, "alListenerfv"), (void*)alListenerfv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alListeneri"), (void*)alListeneri);
    aml->Hook((void*)aml->GetSym(pGTASA, "alListeneriv"), (void*)alListeneriv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alProcessUpdatesSOFT"), (void*)alProcessUpdatesSOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSetConfigMOB"), (void*)ZeroReturn);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSource3dSOFT"), (void*)alSource3dSOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSource3f"), (void*)alSource3f);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSource3i"), (void*)alSource3i);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSource3i64SOFT"), (void*)alSource3i64SOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSourcePause"), (void*)alSourcePause);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSourcePausev"), (void*)alSourcePausev);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSourcePlay"), (void*)alSourcePlay);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSourcePlayv"), (void*)alSourcePlayv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSourceQueueBuffers"), (void*)alSourceQueueBuffers);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSourceRewind"), (void*)alSourceRewind);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSourceRewindv"), (void*)alSourceRewindv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSourceStop"), (void*)alSourceStop);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSourceStopv"), (void*)alSourceStopv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSourceUnqueueBuffers"), (void*)alSourceUnqueueBuffers);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSourcedSOFT"), (void*)alSourcedSOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSourcedvSOFT"), (void*)alSourcedvSOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSourcef"), (void*)alSourcef);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSourcefv"), (void*)alSourcefv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSourcei"), (void*)alSourcei);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSourcei64SOFT"), (void*)alSourcei64SOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSourcei64vSOFT"), (void*)alSourcei64vSOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSourceiv"), (void*)alSourceiv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alSpeedOfSound"), (void*)alSpeedOfSound);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcCaptureCloseDevice"), (void*)alcCaptureCloseDevice);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcCaptureOpenDevice"), (void*)alcCaptureOpenDevice);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcCaptureSamples"), (void*)alcCaptureSamples);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcCaptureStart"), (void*)alcCaptureStart);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcCaptureStop"), (void*)alcCaptureStop);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcCloseDevice"), (void*)alcCloseDevice);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcCreateContext"), (void*)alcCreateContextHook);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcDestroyContext"), (void*)alcDestroyContext);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcDeviceEnableHrtfMOB"), (void*)ZeroReturn);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcGetContextsDevice"), (void*)alcGetContextsDevice);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcGetCurrentContext"), (void*)alcGetCurrentContext);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcGetEnumValue"), (void*)alcGetEnumValue);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcGetError"), (void*)alcGetError);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcGetIntegerv"), (void*)alcGetIntegerv);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcGetProcAddress"), (void*)alcGetProcAddress);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcGetString"), (void*)alcGetString);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcGetThreadContext"), (void*)alcGetThreadContext);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcIsExtensionPresent"), (void*)alcIsExtensionPresent);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcIsRenderFormatSupportedSOFT"), (void*)alcIsRenderFormatSupportedSOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcLoopbackOpenDeviceSOFT"), (void*)alcLoopbackOpenDeviceSOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcMakeContextCurrent"), (void*)alcMakeContextCurrent);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcOpenDevice"), (void*)alcOpenDevice);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcProcessContext"), (void*)alcProcessContext);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcRenderSamplesSOFT"), (void*)alcRenderSamplesSOFT);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcSetThreadContext"), (void*)alcSetThreadContext);
    aml->Hook((void*)aml->GetSym(pGTASA, "alcSuspendContext"), (void*)alcSuspendContext);

    sautils = (ISAUtils*)GetInterface("SAUtils");
    if(sautils != nullptr)
    {
        sautils->AddClickableItem(Audio, "Max Audio Frequency", pCfg44100Frequency->GetBool(), 0, sizeofA(pFreq)-1, pFreq, AudioFrequencyToggled);
        sautils->AddClickableItem(Audio, "HRTF", pCfgHRTF->GetBool(), 0, sizeofA(pYesNo)-1, pYesNo, HRTFToggled);
    }
}