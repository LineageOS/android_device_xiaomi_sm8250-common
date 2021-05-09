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

#ifndef DEFAULT_LOW_PERSISTENCE_MODE_BRIGHTNESS
#define DEFAULT_LOW_PERSISTENCE_MODE_BRIGHTNESS 0x80
#endif

using ::android::base::WriteStringToFile;

namespace aidl {
namespace android {
namespace hardware {
namespace light {

#define JOIN_PATH(a, b)                     a "/" b

#define LED_FILE_BASE                       "/sys/class/leds"

#define RED_LED_FILE_BASE                   JOIN_PATH(LED_FILE_BASE, "red")
#define GREEN_LED_FILE_BASE                 JOIN_PATH(LED_FILE_BASE, "green")
#define BLUE_LED_FILE_BASE                  JOIN_PATH(LED_FILE_BASE, "blue")
#define WHITE_LED_FILE_BASE                 JOIN_PATH(LED_FILE_BASE, "white")

static const std::string kRedBreathFile = JOIN_PATH(RED_LED_FILE_BASE, "breath");
static const std::string kGreenBreathFile = JOIN_PATH(GREEN_LED_FILE_BASE, "breath");
static const std::string kBlueBreathFile = JOIN_PATH(BLUE_LED_FILE_BASE, "breath");
static const std::string kWhiteBreathFile = JOIN_PATH(WHITE_LED_FILE_BASE, "breath");

static const std::string kRedLEDFile = JOIN_PATH(RED_LED_FILE_BASE, "brightness");
static const std::string kGreenLEDFile = JOIN_PATH(GREEN_LED_FILE_BASE, "brightness");
static const std::string kBlueLEDFile = JOIN_PATH(BLUE_LED_FILE_BASE, "brightness");
static const std::string kWhiteLEDFile = JOIN_PATH(WHITE_LED_FILE_BASE, "brightness");

static const std::string kRedDelayOffFile = JOIN_PATH(RED_LED_FILE_BASE, "delay_off");
static const std::string kGreenDelayOffFile = JOIN_PATH(GREEN_LED_FILE_BASE, "delay_off");
static const std::string kBlueDelayOffFile = JOIN_PATH(BLUE_LED_FILE_BASE, "delay_off");
static const std::string kWhiteDelayOffFile = JOIN_PATH(WHITE_LED_FILE_BASE, "delay_off");

static const std::string kRedDelayOnFile = JOIN_PATH(RED_LED_FILE_BASE, "delay_on");
static const std::string kGreenDelayOnFile = JOIN_PATH(GREEN_LED_FILE_BASE, "delay_on");
static const std::string kBlueDelayOnFile = JOIN_PATH(BLUE_LED_FILE_BASE, "delay_on");
static const std::string kWhiteDelayOnFile = JOIN_PATH(WHITE_LED_FILE_BASE, "delay_on");

static const std::string kLCDFile = "/sys/class/backlight/panel0-backlight/brightness";

#define AutoHwLight(light) {.id = (int)light, .type = light, .ordinal = 0}

// List of supported lights
const static std::vector<HwLight> kAvailableLights = {
    AutoHwLight(LightType::BACKLIGHT),
    AutoHwLight(LightType::BATTERY),
    AutoHwLight(LightType::NOTIFICATIONS)
};

Lights::Lights() {
    mWhiteLed = !access(kWhiteLEDFile.c_str(), W_OK);
}

// AIDL methods
ndk::ScopedAStatus Lights::setLightState(int id, const HwLightState& state) {
    switch (id) {
        case (int)LightType::BACKLIGHT:
            setLightBacklight(state);
            break;
        case (int)LightType::BATTERY:
            setLightBattery(state);
            break;
        case (int)LightType::NOTIFICATIONS:
            setLightNotification(state);
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
ndk::ScopedAStatus Lights::setLightBacklight(const HwLightState& state) {
    WriteToFile(kLCDFile, RgbaToBrightness(state.color));
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Lights::setLightBattery(const HwLightState& state) {
    mBattery = state;
    handleSpeakerBatteryLocked();
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Lights::setLightNotification(const HwLightState& state) {
    mNotification = state;
    handleSpeakerBatteryLocked();
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Lights::setSpeakerLightLocked(const HwLightState& state) {
    uint32_t red, green, blue;
    uint32_t blink;
    unsigned int colorRGB;
    uint32_t alpha;
    bool rc = true;

    colorRGB = state.color;

    // Extract brightness from AARRGGBB
    alpha = (colorRGB >> 24) & 0xFF;
    red = (colorRGB >> 16) & 0xFF;
    green = (colorRGB >> 8) & 0xFF;
    blue = colorRGB & 0xFF;

    // Scale RGB brightness if Alpha brightness is not 0xFF
    if (alpha != 0xFF) {
        red = (red * alpha) / 0xFF;
        green = (green * alpha) / 0xFF;
        blue = (blue * alpha) / 0xFF;
    }

    blink = (state.flashOnMs != 0 && state.flashOffMs != 0);

    switch (state.flashMode) {
        case FlashMode::HARDWARE:
            if (mWhiteLed) {
                rc = WriteToFile(kWhiteBreathFile, blink);
            } else {
                if (!!red)
                    rc = WriteToFile(kRedBreathFile, blink);
                if (!!green)
                    rc &= WriteToFile(kGreenBreathFile, blink);
                if (!!blue)
                    rc &= WriteToFile(kBlueBreathFile, blink);
            }
            if (rc)
                break;
        case FlashMode::TIMED:
            if (mWhiteLed) {
                rc = WriteToFile(kWhiteDelayOffFile, state.flashOffMs);
                rc &= WriteToFile(kWhiteDelayOnFile, state.flashOnMs);
            } else {
                if (!!red) {
                    rc = WriteToFile(kRedDelayOffFile, state.flashOffMs);
                    rc &= WriteToFile(kRedDelayOnFile, state.flashOnMs);
                }
                if (!!green) {
                    rc &= WriteToFile(kGreenDelayOffFile, state.flashOffMs);
                    rc &= WriteToFile(kGreenDelayOnFile, state.flashOnMs);
                }
                if (!!blue) {
                    rc &= WriteToFile(kBlueDelayOffFile, state.flashOffMs);
                    rc &= WriteToFile(kBlueDelayOnFile, state.flashOnMs);
                }
            }
            if (rc)
                break;
        case FlashMode::NONE:
        default:
            if (mWhiteLed) {
                rc = WriteToFile(kWhiteLEDFile, RgbaToBrightness(state.color));
            } else {
                rc = WriteToFile(kRedLEDFile, red);
                rc &= WriteToFile(kGreenLEDFile, green);
                rc &= WriteToFile(kBlueLEDFile, blue);
            }
            break;
    }

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Lights::handleSpeakerBatteryLocked() {
    if (IsLit(mBattery.color))
        return setSpeakerLightLocked(mBattery);
    else
        return setSpeakerLightLocked(mNotification);
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
