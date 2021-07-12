/*
 * Copyright (C) 2019 The Android Open Source Project
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

#include "Lights.h"

#include <android-base/file.h>
#include <android-base/logging.h>
#include <fcntl.h>

using ::android::base::WriteStringToFile;

namespace aidl {
namespace android {
namespace hardware {
namespace light {

#define LED_PATH(led)                       "/sys/class/leds/" led "/"

static const std::string led_paths[] {
    [RED] = LED_PATH("red"),
    [GREEN] = LED_PATH("green"),
    [BLUE] = LED_PATH("blue"),
    [WHITE] = LED_PATH("white"),
};

static const std::string kLCDFile = "/sys/class/backlight/panel0-backlight/brightness";

#define AutoHwLight(light) {.id = (int)light, .type = light, .ordinal = 0}

// List of supported lights
const static std::vector<HwLight> kAvailableLights = {
    AutoHwLight(LightType::BACKLIGHT),
    AutoHwLight(LightType::BATTERY),
    AutoHwLight(LightType::NOTIFICATIONS)
};

Lights::Lights() {
    mWhiteLed = !access((led_paths[WHITE] + "brightness").c_str(), W_OK);
}

// AIDL methods
ndk::ScopedAStatus Lights::setLightState(int id, const HwLightState& state) {
    switch (id) {
        case (int)LightType::BACKLIGHT:
            WriteToFile(kLCDFile, RgbaToBrightness(state.color));
            break;
        case (int)LightType::BATTERY:
            mBattery = state;
            handleSpeakerBatteryLocked();
            break;
        case (int)LightType::NOTIFICATIONS:
            mNotification = state;
            handleSpeakerBatteryLocked();
            break;
        default:
            return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
            break;
    }

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Lights::getLights(std::vector<HwLight>* lights) {
    for (auto i = kAvailableLights.begin(); i != kAvailableLights.end(); i++) {
        lights->push_back(*i);
    }
    return ndk::ScopedAStatus::ok();
}

// device methods
void Lights::setSpeakerLightLocked(const HwLightState& state) {
    uint32_t alpha, red, green, blue;
    uint32_t blink;
    bool rc = true;

    // Extract brightness from AARRGGBB
    alpha = (state.color >> 24) & 0xFF;
    red = (state.color >> 16) & 0xFF;
    green = (state.color >> 8) & 0xFF;
    blue = state.color & 0xFF;

    // Scale RGB brightness if Alpha brightness is not 0xFF
    if (alpha != 0xFF) {
        red = (red * alpha) / 0xFF;
        green = (green * alpha) / 0xFF;
        blue = (blue * alpha) / 0xFF;
    }

    blink = (state.flashOnMs != 0 && state.flashOffMs != 0);

    switch (state.flashMode) {
        case FlashMode::HARDWARE:
        case FlashMode::TIMED:
            if (mWhiteLed) {
                rc = setLedBreath(WHITE, blink);
            } else {
                if (!!red)
                    rc = setLedBreath(RED, blink);
                if (!!green)
                    rc &= setLedBreath(GREEN, blink);
                if (!!blue)
                    rc &= setLedBreath(BLUE, blink);
            }
            if (rc)
                break;
            FALLTHROUGH_INTENDED;
        case FlashMode::NONE:
        default:
            if (mWhiteLed) {
                rc = setLedBrightness(WHITE, RgbaToBrightness(state.color));
            } else {
                rc = setLedBrightness(RED, red);
                rc &= setLedBrightness(GREEN, green);
                rc &= setLedBrightness(BLUE, blue);
            }
            break;
    }

    return;
}

void Lights::handleSpeakerBatteryLocked() {
    if (IsLit(mBattery.color))
        return setSpeakerLightLocked(mBattery);
    else
        return setSpeakerLightLocked(mNotification);
}

bool Lights::setLedBreath(led_type led, uint32_t value) {
    return WriteToFile(led_paths[led] + "breath", value);
}

bool Lights::setLedBrightness(led_type led, uint32_t value) {
    return WriteToFile(led_paths[led] + "brightness", value);
}

// Utils
bool Lights::IsLit(uint32_t color) {
    return color & 0x00ffffff;
}

uint32_t Lights::RgbaToBrightness(uint32_t color) {
    // Extract brightness from AARRGGBB.
    uint32_t alpha = (color >> 24) & 0xFF;

    // Retrieve each of the RGB colors
    uint32_t red = (color >> 16) & 0xFF;
    uint32_t green = (color >> 8) & 0xFF;
    uint32_t blue = color & 0xFF;

    // Scale RGB colors if a brightness has been applied by the user
    if (alpha != 0xFF) {
        red = red * alpha / 0xFF;
        green = green * alpha / 0xFF;
        blue = blue * alpha / 0xFF;
    }

    return (77 * red + 150 * green + 29 * blue) >> 8;
}

// Write value to path and close file.
bool Lights::WriteToFile(const std::string& path, uint32_t content) {
    return WriteStringToFile(std::to_string(content), path);
}

}  // namespace light
}  // namespace hardware
}  // namespace android
}  // namespace aidl
