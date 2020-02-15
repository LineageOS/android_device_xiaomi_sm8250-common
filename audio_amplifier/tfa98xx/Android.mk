#
# Copyright (C) 2015 The CyanogenMod Project
# Copyright (C) 2020-2021 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := audio_amplifier.tfa98xx.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SRC_FILES := tfa98xx.c
LOCAL_VENDOR_MODULE := true

LOCAL_C_INCLUDES += \
    $(call include-path-for, audio-route) \
    $(call include-path-for, audio-utils) \
    $(call project-path-for, qcom-audio)/hal \
    $(call project-path-for, qcom-audio)/hal/audio_extn \
    $(call project-path-for, qcom-audio)/hal/msm8974 \
    external/tinycompress/include

LOCAL_HEADER_LIBRARIES += \
    generated_kernel_headers \
    libhardware_headers

LOCAL_SHARED_LIBRARIES += \
    audio.primary.$(TARGET_BOARD_PLATFORM) \
    libcutils \
    liblog \
    libtinyalsa

include $(BUILD_SHARED_LIBRARY)
