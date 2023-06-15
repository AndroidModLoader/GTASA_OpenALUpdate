LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cpp .cc
LOCAL_MODULE    := OpenALUpdate

# Google Oboe
FILE_LIST := $(wildcard $(LOCAL_PATH)/oboe/src/aaudio/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/oboe/src/common/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/oboe/src/fifo/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/oboe/src/flowgraph/*.cpp) $(wildcard $(LOCAL_PATH)/oboe/src/flowgraph/resampler/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/oboe/src/opensles/*.cpp)

# kcat's OpenAL Soft
FILE_LIST += $(wildcard $(LOCAL_PATH)/openal-soft/al/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/openal-soft/al/effects/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/openal-soft/alc/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/openal-soft/alc/effects/*.cpp)
#FILE_LIST += $(wildcard $(LOCAL_PATH)/openal-soft/core/*.cpp)
#FILE_LIST += $(wildcard $(LOCAL_PATH)/openal-soft/core/filters/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/openal-soft/common/*.cpp)

# Finalize wildcards
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

# One more OAL's sources
LOCAL_SRC_FILES += openal-soft/core/mixer/mixer_neon.cpp \
                   openal-soft/alc/backends/base.cpp \
                   openal-soft/alc/backends/null.cpp \
                   openal-soft/alc/backends/oboe.cpp \
                   openal-soft/alc/backends/loopback.cpp
                   
LOCAL_SRC_FILES += \
    openal-soft/core/ambdec.cpp \
    openal-soft/core/ambidefs.cpp \
    openal-soft/core/bformatdec.cpp \
    openal-soft/core/bs2b.cpp \
    openal-soft/core/bsinc_tables.cpp \
    openal-soft/core/buffer_storage.cpp \
    openal-soft/core/context.cpp \
    openal-soft/core/converter.cpp \
    openal-soft/core/cpu_caps.cpp \
    openal-soft/core/cubic_tables.cpp \
    openal-soft/core/devformat.cpp \
    openal-soft/core/device.cpp \
    openal-soft/core/effectslot.cpp \
    openal-soft/core/except.cpp \
    openal-soft/core/filters/biquad.cpp \
    openal-soft/core/filters/nfc.cpp \
    openal-soft/core/filters/splitter.cpp \
    openal-soft/core/fmt_traits.cpp \
    openal-soft/core/fpu_ctrl.cpp \
    openal-soft/core/helpers.cpp \
    openal-soft/core/hrtf.cpp \
    openal-soft/core/logging.cpp \
    openal-soft/core/mastering.cpp \
    openal-soft/core/mixer.cpp \
    openal-soft/core/uhjfilter.cpp \
    openal-soft/core/uiddefs.cpp \
    openal-soft/core/voice.cpp \
    openal-soft/core/mixer/mixer_c.cpp

# Just a main mod thingies
LOCAL_SRC_FILES += main.cpp mod/logger.cpp mod/config.cpp

LOCAL_CFLAGS += -O2 -mfloat-abi=softfp -DNDEBUG -std=c++17 -fexceptions -DRESTRICT=__restrict__ -mfpu=neon -DAL_LIBTYPE_STATIC -DAL_ALEXT_PROTOTYPES
LOCAL_C_INCLUDES += $(LOCAL_PATH)/openal-soft $(LOCAL_PATH)/openal-soft/include $(LOCAL_PATH)/openal-soft/common $(LOCAL_PATH)/oboe/include $(LOCAL_PATH)/oboe/src
LOCAL_LDLIBS += -llog -lOpenSLES
include $(BUILD_SHARED_LIBRARY)
