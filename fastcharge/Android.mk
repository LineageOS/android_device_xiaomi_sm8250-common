#
# Copyright (C) 2023 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    FastCharge.cpp \
    service.cpp

LOCAL_SHARED_LIBRARIES := \
    libbase \
    libbinder \
    libhidlbase \
    libutils \
    vendor.lineage.fastcharge@1.0

LOCAL_MODULE := vendor.lineage.fastcharge@1.0-service.xiaomi_sm8250
LOCAL_INIT_RC := vendor.lineage.fastcharge@1.0-service.xiaomi_sm8250.rc
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MODULE_TAGS := optional
LOCAL_VINTF_FRAGMENTS := vendor.lineage.fastcharge@1.0-service.xiaomi_sm8250.xml
LOCAL_VENDOR_MODULE := true

include $(BUILD_EXECUTABLE)
