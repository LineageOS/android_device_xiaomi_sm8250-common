/*
 * Copyright (C) 2019-2021 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "vendor.lineage.biometrics.fingerprint.inscreen@1.0-service.xiaomi_kona"

#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>

#include "FingerprintInscreen.h"

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

using vendor::lineage::biometrics::fingerprint::inscreen::V1_0::IFingerprintInscreen;
using vendor::lineage::biometrics::fingerprint::inscreen::V1_0::implementation::FingerprintInscreen;

using android::OK;
using android::status_t;

int main() {
    android::sp<IFingerprintInscreen> service = new FingerprintInscreen();

    configureRpcThreadpool(1, true);

    status_t status = service->registerAsService();
    if (status != OK) {
        LOG(ERROR) << "Cannot register FOD HAL service.";
        return 1;
    }

    LOG(INFO) << "FOD HAL service ready.";

    joinRpcThreadpool();

    LOG(ERROR) << "FOD HAL service failed to join thread pool.";
    return 1;
}
