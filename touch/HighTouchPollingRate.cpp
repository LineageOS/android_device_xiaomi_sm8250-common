/*
 * Copyright (C) 2022 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "vendor.lineage.touch@1.0-service.xiaomi_kona"

#include "HighTouchPollingRate.h"

#include <fstream>

namespace vendor {
namespace lineage {
namespace touch {
namespace V1_0 {
namespace implementation {

const std::string kHighTouchPollingPath = "/sys/devices/virtual/touch/touch_dev/bump_sample_rate";

Return<bool> HighTouchPollingRate::isEnabled() {
    std::ifstream file(kHighTouchPollingPath);
    int enabled;
    file >> enabled;
    return enabled == 1;
}

Return<bool> HighTouchPollingRate::setEnabled(bool enabled) {
    std::ofstream file(kHighTouchPollingPath);
    file << (enabled ? "1" : "0");
    return !file.fail();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace touch
}  // namespace lineage
}  // namespace vendor
