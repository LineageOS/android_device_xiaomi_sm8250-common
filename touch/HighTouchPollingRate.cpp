/*
 * Copyright (C) 2022 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "HighTouchPollingRateService"

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

    if(enabled == 1)
        return true;

    return false;
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
