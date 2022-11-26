LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cpp .cc
LOCAL_MODULE    := OpenALUpdate

# Google Oboe
FILE_LIST := $(wildcard $(LOCAL_PATH)/oboe/aaudio/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/oboe/common/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/oboe/fifo/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/oboe/flowgraph/*.cpp) $(wildcard $(LOCAL_PATH)/oboe/flowgraph/resampler/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/oboe/opensles/*.cpp)

# kcat's OpenAL Soft
FILE_LIST += $(wildcard $(LOCAL_PATH)/al/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/al/effects/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/alc/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/alc/effects/*.cpp)
#FILE_LIST += $(wildcard $(LOCAL_PATH)/core/*.cpp)
#FILE_LIST += $(wildcard $(LOCAL_PATH)/core/filters/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/common/*.cpp)

# Finalize wildcards
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

# One more OAL's sources
LOCAL_SRC_FILES += core/mixer/mixer_neon.cpp \
                   alc/backends/base.cpp \
                   alc/backends/null.cpp \
                   alc/backends/oboe.cpp \
                   alc/backends/loopback.cpp
                   
LOCAL_SRC_FILES += \
    core/ambdec.cpp \
    core/ambidefs.cpp \
    core/bformatdec.cpp \
    core/bs2b.cpp \
    core/bsinc_tables.cpp \
    core/buffer_storage.cpp \
    core/context.cpp \
    core/converter.cpp \
    core/cpu_caps.cpp \
    core/devformat.cpp \
    core/device.cpp \
    core/effectslot.cpp \
    core/except.cpp \
    core/filters/biquad.cpp \
    core/filters/nfc.cpp \
    core/filters/splitter.cpp \
    core/fmt_traits.cpp \
    core/fpu_ctrl.cpp \
    core/helpers.cpp \
    core/hrtf.cpp \
    core/logging.cpp \
    core/mastering.cpp \
    core/mixer.cpp \
    core/uhjfilter.cpp \
    core/uiddefs.cpp \
    core/voice.cpp \
    core/mixer/mixer_c.cpp

# Just a main mod thingies
LOCAL_SRC_FILES += main.cpp mod/logger.cpp mod/config.cpp

LOCAL_CFLAGS += -O2 -mfloat-abi=softfp -DNDEBUG -std=c++17 -fexceptions -DRESTRICT=__restrict__ -mfpu=neon -DAL_LIBTYPE_STATIC -DAL_ALEXT_PROTOTYPES
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include $(LOCAL_PATH)/common $(LOCAL_PATH)/oboe
LOCAL_LDLIBS += -llog -lOpenSLES
include $(BUILD_SHARED_LIBRARY)
