LOCAL_PATH := $(call my-dir)
rwildcard =: $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cpp .cc
LOCAL_MODULE    := OpenALUpdate
LOCAL_SRC_FILES := main.cpp mod/logger.cpp mod/config.cpp
LOCAL_SRC_FILES += $(call rwildcard,core/,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,al/,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,alc/,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,common/,*.cpp)
LOCAL_CFLAGS += -O2 -mfloat-abi=softfp -DNDEBUG -std=c++17 -fexceptions -DRESTRICT=__restrict__ -DAL_LIBTYPE_STATIC -DAL_ALEXT_PROTOTYPES
LOCAL_C_INCLUDES += ./include ./common
LOCAL_LDLIBS += -llog -lOpenSLES
include $(BUILD_SHARED_LIBRARY)