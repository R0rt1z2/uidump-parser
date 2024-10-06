LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := uidump-parser

LOCAL_SRC_FILES := \
	src/main.cpp \
    src/tinyxml2/tinyxml2.cpp

LOCAL_CFLAGS += \
    -Wno-pointer-sign \
    -Wno-int-to-pointer-cast

include $(BUILD_EXECUTABLE)

$(call import-add-path, $(LOCAL_PATH))