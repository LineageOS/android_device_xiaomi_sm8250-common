/*
 * Copyright (C) 2023 The LineageOS Project
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

#define LOG_TAG "fastcharge@1.0-service.xiaomi_sm8250"

#include "FastCharge.h"
#include <android-base/logging.h>
#include <cutils/properties.h>

#include <fstream>
#include <iostream>

namespace vendor {
namespace lineage {
namespace fastcharge {
namespace V1_0 {
namespace implementation {

static constexpr const char *kFastChargingProp =
    "persist.vendor.fastcharging.enabled";

Return<bool> FastCharge::isEnabled() {
  return property_get_bool(kFastChargingProp, true);
}

Return<bool> FastCharge::setEnabled(bool enable) {
  property_set(kFastChargingProp, enable ? "true" : "false");

  return enable;
}

} // namespace implementation
} // namespace V1_0
} // namespace fastcharge
} // namespace lineage
} // namespace vendor
