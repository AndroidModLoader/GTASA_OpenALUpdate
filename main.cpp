#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#define sizeofA(__aVar)  ((int)(sizeof(__aVar)/sizeof(__aVar[0])))

#include <include/AL/al.h>
#include <include/AL/alc.h>
#include <include/AL/alext.h>
#include <include/AL/efx.h>

#include "isautils.h"
ISAUtils* sautils = NULL;

MYMODCFG(net.rusjj.gtasa.openal, OpenAL Soft, 1.2, kcat & TheOfficialFloW & RusJJ)

#define BUILD_ATTR() const ALCint attr[] = { \
        ALC_FREQUENCY, pCfg44100Frequency->GetBool() ? 44100 : 22050, \
        ALC_HRTF_SOFT, pCfgHRTF->GetBool(), \
        0 } // END OF THE LIST


void* hGTASA = NULL;
uintptr_t pGTASAAddr = 0;
ALCdevice* pSoundDevice = NULL;
ALCcontext* pDeviceContext = NULL;
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
ConfigEntry* pCfgDontPauseSounds;

bool UpdateALDevice()
{
    BUILD_ATTR();
    return alcResetDeviceSOFT(pSoundDevice, attr);
}
ALCcontext *alcCreateContextHook(ALCdevice* device, const ALCint* attributes)
{
    if(pSoundDevice != NULL) logger->Error("alcCreateContext created once more and with a different ALC Device?");
    pSoundDevice = device;
    BUILD_ATTR();
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
void SoundsPauseToggled(int oldVal, int newVal, void* data)
{
    pCfgDontPauseSounds->SetInt(newVal);
    cfg->Save();
}


bool bPaused = false;
DECL_HOOKv(PauseOpenAL, uintptr_t self, int doPause)
{
    bPaused = (doPause != 0);
    if(!pSoundDevice || pCfgDontPauseSounds->GetBool()) return;
    doPause ? alcDevicePauseSOFT(pSoundDevice) : alcDeviceResumeSOFT(pSoundDevice);
}
DECL_HOOKv(InitOpenALListener, void* self)
{
    InitOpenALListener(self);

    alDistanceModel(AL_EXPONENT_DISTANCE_CLAMPED);
}
int ZeroReturn(void* args, ...) { return 0; }

extern "C" void OnModLoad()
{
    logger->SetTag("AML: OpenAL Soft");
    pCfg44100Frequency = cfg->Bind("44100hz", true);
    pCfgHRTF = cfg->Bind("HRTF", false);
    pCfgDontPauseSounds = cfg->Bind("DontPauseSounds", false);

    hGTASA = aml->GetLibHandle("libGTASA.so"); // aml->GetLib(...) returns address only! We need a HANDLE for GetSym!
    pGTASAAddr = aml->GetLib("libGTASA.so");

    aml->Redirect(aml->GetSym(hGTASA, "alAuxiliaryEffectSlotf"), (uintptr_t)alAuxiliaryEffectSlotf);
    aml->Redirect(aml->GetSym(hGTASA, "alAuxiliaryEffectSlotfv"), (uintptr_t)alAuxiliaryEffectSlotfv);
    aml->Redirect(aml->GetSym(hGTASA, "alAuxiliaryEffectSloti"), (uintptr_t)alAuxiliaryEffectSloti);
    aml->Redirect(aml->GetSym(hGTASA, "alAuxiliaryEffectSlotiv"), (uintptr_t)alAuxiliaryEffectSlotiv);
    aml->Redirect(aml->GetSym(hGTASA, "alBuffer3f"), (uintptr_t)alBuffer3f);
    aml->Redirect(aml->GetSym(hGTASA, "alBuffer3i"), (uintptr_t)alBuffer3i);
    aml->Redirect(aml->GetSym(hGTASA, "alBufferData"), (uintptr_t)alBufferData);
    aml->Redirect(aml->GetSym(hGTASA, "alBufferSamplesSOFT"), (uintptr_t)alBufferSamplesSOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alBufferSubDataSOFT"), (uintptr_t)alBufferSubDataSOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alBufferSubSamplesSOFT"), (uintptr_t)alBufferSubSamplesSOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alBufferf"), (uintptr_t)alBufferf);
    aml->Redirect(aml->GetSym(hGTASA, "alBufferfv"), (uintptr_t)alBufferfv);
    aml->Redirect(aml->GetSym(hGTASA, "alBufferi"), (uintptr_t)alBufferi);
    //aml->Redirect(aml->GetSym(hGTASA, "alBufferiv"), (uintptr_t)alBufferiv); // crash? SEGV_ACCERR, not used anyway so i dont care... BUT HOW DOES THAT HAPPEN?!
    aml->Redirect(aml->GetSym(hGTASA, "alDeferUpdatesSOFT"), (uintptr_t)alDeferUpdatesSOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alDeleteAuxiliaryEffectSlots"), (uintptr_t)alDeleteAuxiliaryEffectSlots);
    aml->Redirect(aml->GetSym(hGTASA, "alDeleteBuffers"), (uintptr_t)alDeleteBuffers);
    aml->Redirect(aml->GetSym(hGTASA, "alDeleteEffects"), (uintptr_t)alDeleteEffects);
    aml->Redirect(aml->GetSym(hGTASA, "alDeleteFilters"), (uintptr_t)alDeleteFilters);
    aml->Redirect(aml->GetSym(hGTASA, "alDeleteSources"), (uintptr_t)alDeleteSources);
    aml->Redirect(aml->GetSym(hGTASA, "alDisable"), (uintptr_t)alDisable);
    aml->Redirect(aml->GetSym(hGTASA, "alDistanceModel"), (uintptr_t)alDistanceModel);
    aml->Redirect(aml->GetSym(hGTASA, "alDopplerFactor"), (uintptr_t)alDopplerFactor);
    aml->Redirect(aml->GetSym(hGTASA, "alDopplerVelocity"), (uintptr_t)alDopplerVelocity);
    aml->Redirect(aml->GetSym(hGTASA, "alEffectf"), (uintptr_t)alEffectf);
    aml->Redirect(aml->GetSym(hGTASA, "alEffectfv"), (uintptr_t)alEffectfv);
    aml->Redirect(aml->GetSym(hGTASA, "alEffecti"), (uintptr_t)alEffecti);
    aml->Redirect(aml->GetSym(hGTASA, "alEffectiv"), (uintptr_t)alEffectiv);
    aml->Redirect(aml->GetSym(hGTASA, "alEnable"), (uintptr_t)alEnable);
    aml->Redirect(aml->GetSym(hGTASA, "alFilterf"), (uintptr_t)alFilterf);
    aml->Redirect(aml->GetSym(hGTASA, "alFilterfv"), (uintptr_t)alFilterfv);
    aml->Redirect(aml->GetSym(hGTASA, "alFilteri"), (uintptr_t)alFilteri);
    aml->Redirect(aml->GetSym(hGTASA, "alFilteriv"), (uintptr_t)alFilteriv);
    aml->Redirect(aml->GetSym(hGTASA, "alGenBuffers"), (uintptr_t)alGenBuffers);
    aml->Redirect(aml->GetSym(hGTASA, "alGenEffects"), (uintptr_t)alGenEffects);
    aml->Redirect(aml->GetSym(hGTASA, "alGenFilters"), (uintptr_t)alGenFilters);
    aml->Redirect(aml->GetSym(hGTASA, "alGenSources"), (uintptr_t)alGenSources);
    aml->Redirect(aml->GetSym(hGTASA, "alGetAuxiliaryEffectSlotf"), (uintptr_t)alGetAuxiliaryEffectSlotf);
    aml->Redirect(aml->GetSym(hGTASA, "alGetAuxiliaryEffectSlotfv"), (uintptr_t)alGetAuxiliaryEffectSlotfv);
    aml->Redirect(aml->GetSym(hGTASA, "alGetAuxiliaryEffectSloti"), (uintptr_t)alGetAuxiliaryEffectSloti);
    aml->Redirect(aml->GetSym(hGTASA, "alGetAuxiliaryEffectSlotiv"), (uintptr_t)alGetAuxiliaryEffectSlotiv);
    aml->Redirect(aml->GetSym(hGTASA, "alGetBoolean"), (uintptr_t)alGetBoolean);
    aml->Redirect(aml->GetSym(hGTASA, "alGetBooleanv"), (uintptr_t)alGetBooleanv);
    aml->Redirect(aml->GetSym(hGTASA, "alGetBuffer3f"), (uintptr_t)alGetBuffer3f);
    aml->Redirect(aml->GetSym(hGTASA, "alGetBuffer3i"), (uintptr_t)alGetBuffer3i);
    aml->Redirect(aml->GetSym(hGTASA, "alGetBufferSamplesSOFT"), (uintptr_t)alGetBufferSamplesSOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alGetBufferf"), (uintptr_t)alGetBufferf);
    aml->Redirect(aml->GetSym(hGTASA, "alGetBufferfv"), (uintptr_t)alGetBufferfv);
    aml->Redirect(aml->GetSym(hGTASA, "alGetBufferi"), (uintptr_t)alGetBufferi);
    aml->Redirect(aml->GetSym(hGTASA, "alGetBufferiv"), (uintptr_t)alGetBufferiv);
    aml->Redirect(aml->GetSym(hGTASA, "alGetDouble"), (uintptr_t)alGetDouble);
    aml->Redirect(aml->GetSym(hGTASA, "alGetDoublev"), (uintptr_t)alGetDoublev);
    aml->Redirect(aml->GetSym(hGTASA, "alGetEffectf"), (uintptr_t)alGetEffectf);
    aml->Redirect(aml->GetSym(hGTASA, "alGetEffectfv"), (uintptr_t)alGetEffectfv);
    aml->Redirect(aml->GetSym(hGTASA, "alGetEffecti"), (uintptr_t)alGetEffecti);
    aml->Redirect(aml->GetSym(hGTASA, "alGetEffectiv"), (uintptr_t)alGetEffectiv);
    aml->Redirect(aml->GetSym(hGTASA, "alGetEnumValue"), (uintptr_t)alGetEnumValue);
    aml->Redirect(aml->GetSym(hGTASA, "alGetError"), (uintptr_t)alGetError);
    aml->Redirect(aml->GetSym(hGTASA, "alGetFilterf"), (uintptr_t)alGetFilterf);
    aml->Redirect(aml->GetSym(hGTASA, "alGetFilterfv"), (uintptr_t)alGetFilterfv);
    aml->Redirect(aml->GetSym(hGTASA, "alGetFilteri"), (uintptr_t)alGetFilteri);
    aml->Redirect(aml->GetSym(hGTASA, "alGetFilteriv"), (uintptr_t)alGetFilteriv);
    aml->Redirect(aml->GetSym(hGTASA, "alGetFloat"), (uintptr_t)alGetFloat);
    aml->Redirect(aml->GetSym(hGTASA, "alGetFloatv"), (uintptr_t)alGetFloatv);
    aml->Redirect(aml->GetSym(hGTASA, "alGetInteger"), (uintptr_t)alGetInteger);
    aml->Redirect(aml->GetSym(hGTASA, "alGetIntegerv"), (uintptr_t)alGetIntegerv);
    aml->Redirect(aml->GetSym(hGTASA, "alGetListener3f"), (uintptr_t)alGetListener3f);
    aml->Redirect(aml->GetSym(hGTASA, "alGetListener3i"), (uintptr_t)alGetListener3i);
    aml->Redirect(aml->GetSym(hGTASA, "alGetListenerf"), (uintptr_t)alGetListenerf);
    aml->Redirect(aml->GetSym(hGTASA, "alGetListenerfv"), (uintptr_t)alGetListenerfv);
    aml->Redirect(aml->GetSym(hGTASA, "alGetListeneri"), (uintptr_t)alGetListeneri);
    aml->Redirect(aml->GetSym(hGTASA, "alGetListeneriv"), (uintptr_t)alGetListeneriv);
    aml->Redirect(aml->GetSym(hGTASA, "alGetProcAddress"), (uintptr_t)alGetProcAddress);
    aml->Redirect(aml->GetSym(hGTASA, "alGetSource3dSOFT"), (uintptr_t)alGetSource3dSOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alGetSource3f"), (uintptr_t)alGetSource3f);
    aml->Redirect(aml->GetSym(hGTASA, "alGetSource3i"), (uintptr_t)alGetSource3i);
    aml->Redirect(aml->GetSym(hGTASA, "alGetSource3i64SOFT"), (uintptr_t)alGetSource3i64SOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alGetSourcedSOFT"), (uintptr_t)alGetSourcedSOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alGetSourcedvSOFT"), (uintptr_t)alGetSourcedvSOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alGetSourcef"), (uintptr_t)alGetSourcef);
    aml->Redirect(aml->GetSym(hGTASA, "alGetSourcefv"), (uintptr_t)alGetSourcefv);
    aml->Redirect(aml->GetSym(hGTASA, "alGetSourcei"), (uintptr_t)alGetSourcei);
    aml->Redirect(aml->GetSym(hGTASA, "alGetSourcei64SOFT"), (uintptr_t)alGetSourcei64SOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alGetSourcei64vSOFT"), (uintptr_t)alGetSourcei64vSOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alGetSourceiv"), (uintptr_t)alGetSourceiv);
    aml->Redirect(aml->GetSym(hGTASA, "alGetString"), (uintptr_t)alGetString);
    aml->Redirect(aml->GetSym(hGTASA, "alIsAuxiliaryEffectSlot"), (uintptr_t)alIsAuxiliaryEffectSlot);
    aml->Redirect(aml->GetSym(hGTASA, "alIsBuffer"), (uintptr_t)alIsBuffer);
    aml->Redirect(aml->GetSym(hGTASA, "alIsBufferFormatSupportedSOFT"), (uintptr_t)alIsBufferFormatSupportedSOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alIsEffect"), (uintptr_t)alIsEffect);
    aml->Redirect(aml->GetSym(hGTASA, "alIsEnabled"), (uintptr_t)alIsEnabled);
    aml->Redirect(aml->GetSym(hGTASA, "alIsExtensionPresent"), (uintptr_t)alIsExtensionPresent);
    aml->Redirect(aml->GetSym(hGTASA, "alIsFilter"), (uintptr_t)alIsFilter);
    aml->Redirect(aml->GetSym(hGTASA, "alIsSource"), (uintptr_t)alIsSource);
    aml->Redirect(aml->GetSym(hGTASA, "alListener3f"), (uintptr_t)alListener3f);
    aml->Redirect(aml->GetSym(hGTASA, "alListener3i"), (uintptr_t)alListener3i);
    aml->Redirect(aml->GetSym(hGTASA, "alListenerf"), (uintptr_t)alListenerf);
    aml->Redirect(aml->GetSym(hGTASA, "alListenerfv"), (uintptr_t)alListenerfv);
    aml->Redirect(aml->GetSym(hGTASA, "alListeneri"), (uintptr_t)alListeneri);
    aml->Redirect(aml->GetSym(hGTASA, "alListeneriv"), (uintptr_t)alListeneriv);
    aml->Redirect(aml->GetSym(hGTASA, "alProcessUpdatesSOFT"), (uintptr_t)alProcessUpdatesSOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alSource3dSOFT"), (uintptr_t)alSource3dSOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alSource3f"), (uintptr_t)alSource3f);
    aml->Redirect(aml->GetSym(hGTASA, "alSource3i"), (uintptr_t)alSource3i);
    aml->Redirect(aml->GetSym(hGTASA, "alSource3i64SOFT"), (uintptr_t)alSource3i64SOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alSourcePause"), (uintptr_t)alSourcePause);
    aml->Redirect(aml->GetSym(hGTASA, "alSourcePausev"), (uintptr_t)alSourcePausev);
    aml->Redirect(aml->GetSym(hGTASA, "alSourcePlay"), (uintptr_t)alSourcePlay);
    aml->Redirect(aml->GetSym(hGTASA, "alSourcePlayv"), (uintptr_t)alSourcePlayv);
    aml->Redirect(aml->GetSym(hGTASA, "alSourceQueueBuffers"), (uintptr_t)alSourceQueueBuffers);
    aml->Redirect(aml->GetSym(hGTASA, "alSourceRewind"), (uintptr_t)alSourceRewind);
    aml->Redirect(aml->GetSym(hGTASA, "alSourceRewindv"), (uintptr_t)alSourceRewindv);
    aml->Redirect(aml->GetSym(hGTASA, "alSourceStop"), (uintptr_t)alSourceStop);
    aml->Redirect(aml->GetSym(hGTASA, "alSourceStopv"), (uintptr_t)alSourceStopv);
    aml->Redirect(aml->GetSym(hGTASA, "alSourceUnqueueBuffers"), (uintptr_t)alSourceUnqueueBuffers);
    aml->Redirect(aml->GetSym(hGTASA, "alSourcedSOFT"), (uintptr_t)alSourcedSOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alSourcedvSOFT"), (uintptr_t)alSourcedvSOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alSourcef"), (uintptr_t)alSourcef);
    aml->Redirect(aml->GetSym(hGTASA, "alSourcefv"), (uintptr_t)alSourcefv);
    aml->Redirect(aml->GetSym(hGTASA, "alSourcei"), (uintptr_t)alSourcei);
    aml->Redirect(aml->GetSym(hGTASA, "alSourcei64SOFT"), (uintptr_t)alSourcei64SOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alSourcei64vSOFT"), (uintptr_t)alSourcei64vSOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alSourceiv"), (uintptr_t)alSourceiv);
    aml->Redirect(aml->GetSym(hGTASA, "alSpeedOfSound"), (uintptr_t)alSpeedOfSound);
    aml->Redirect(aml->GetSym(hGTASA, "alcCaptureCloseDevice"), (uintptr_t)alcCaptureCloseDevice);
    aml->Redirect(aml->GetSym(hGTASA, "alcCaptureOpenDevice"), (uintptr_t)alcCaptureOpenDevice);
    aml->Redirect(aml->GetSym(hGTASA, "alcCaptureSamples"), (uintptr_t)alcCaptureSamples);
    aml->Redirect(aml->GetSym(hGTASA, "alcCaptureStart"), (uintptr_t)alcCaptureStart);
    aml->Redirect(aml->GetSym(hGTASA, "alcCaptureStop"), (uintptr_t)alcCaptureStop);
    aml->Redirect(aml->GetSym(hGTASA, "alcCloseDevice"), (uintptr_t)alcCloseDevice);
    aml->Redirect(aml->GetSym(hGTASA, "alcCreateContext"), (uintptr_t)alcCreateContextHook);
    aml->Redirect(aml->GetSym(hGTASA, "alcDestroyContext"), (uintptr_t)alcDestroyContext);
    aml->Redirect(aml->GetSym(hGTASA, "alcGetContextsDevice"), (uintptr_t)alcGetContextsDevice);
    aml->Redirect(aml->GetSym(hGTASA, "alcGetCurrentContext"), (uintptr_t)alcGetCurrentContext);
    aml->Redirect(aml->GetSym(hGTASA, "alcGetEnumValue"), (uintptr_t)alcGetEnumValue);
    aml->Redirect(aml->GetSym(hGTASA, "alcGetError"), (uintptr_t)alcGetError);
    aml->Redirect(aml->GetSym(hGTASA, "alcGetIntegerv"), (uintptr_t)alcGetIntegerv);
    aml->Redirect(aml->GetSym(hGTASA, "alcGetProcAddress"), (uintptr_t)alcGetProcAddress);
    aml->Redirect(aml->GetSym(hGTASA, "alcGetString"), (uintptr_t)alcGetString);
    aml->Redirect(aml->GetSym(hGTASA, "alcGetThreadContext"), (uintptr_t)alcGetThreadContext);
    aml->Redirect(aml->GetSym(hGTASA, "alcIsExtensionPresent"), (uintptr_t)alcIsExtensionPresent);
    aml->Redirect(aml->GetSym(hGTASA, "alcIsRenderFormatSupportedSOFT"), (uintptr_t)alcIsRenderFormatSupportedSOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alcLoopbackOpenDeviceSOFT"), (uintptr_t)alcLoopbackOpenDeviceSOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alcMakeContextCurrent"), (uintptr_t)alcMakeContextCurrent);
    aml->Redirect(aml->GetSym(hGTASA, "alcOpenDevice"), (uintptr_t)alcOpenDevice);
    aml->Redirect(aml->GetSym(hGTASA, "alcProcessContext"), (uintptr_t)alcProcessContext);
    aml->Redirect(aml->GetSym(hGTASA, "alcRenderSamplesSOFT"), (uintptr_t)alcRenderSamplesSOFT);
    aml->Redirect(aml->GetSym(hGTASA, "alcSetThreadContext"), (uintptr_t)alcSetThreadContext);
    aml->Redirect(aml->GetSym(hGTASA, "alcSuspendContext"), (uintptr_t)alcSuspendContext);

    // Something by WarDrum
    aml->Hook((void*)aml->GetSym(hGTASA, "alSetConfigMOB"), (void*)ZeroReturn);

    // Hooks. Obviously.
    HOOK(PauseOpenAL, aml->GetSym(hGTASA, "_ZN16CAEAudioHardware11PauseOpenALEi"));
    HOOKPLT(InitOpenALListener, pGTASAAddr + 0x6752B0); // aml->GetSym(hGTASA, "_ZN16CAEAudioHardware18InitOpenALListenerEv")

    sautils = (ISAUtils*)GetInterface("SAUtils");
    if(sautils != NULL)
    {
        sautils->AddClickableItem(SetType_Audio, "Max Audio Frequency", pCfg44100Frequency->GetBool(), 0, sizeofA(pFreq)-1, pFreq, AudioFrequencyToggled, NULL);
        sautils->AddClickableItem(SetType_Audio, "HRTF", pCfgHRTF->GetBool(), 0, sizeofA(pYesNo)-1, pYesNo, HRTFToggled, NULL);
        sautils->AddClickableItem(SetType_Audio, "Dont pause sounds", pCfgDontPauseSounds->GetBool(), 0, sizeofA(pYesNo)-1, pYesNo, SoundsPauseToggled, NULL);
    }
}
