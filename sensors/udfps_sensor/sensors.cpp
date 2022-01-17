/*
 * Copyright (C) 2022 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sensors.h"

#define LOG_TAG "UdfpsSensor"

#include <android-base/logging.h>
#include <errno.h>
#include <string.h>

static const struct sensor_t SENSORS_LIST[] = {
    {
        .name = "Virtual UDFPS sensor",
        .vendor = "The LineageOS Project",
        .version = 1, 
        .handle = 0,
        .type = SENSOR_TYPE_UDFPS_SENSOR,
        .maxRange = 1,
        .resolution = 1,
        .power = 0,
        .minDelay = -1, // one-shot
        .fifoReservedEventCount = 0,
        .fifoMaxEventCount = 1,
        .stringType = "org.lineageos.sensor.virtualudfps",
        .requiredPermission = "",
        .maxDelay = 0,
        .flags = SENSOR_FLAG_WAKE_UP | SENSOR_FLAG_ONE_SHOT_MODE,
        .reserved = {},
    },
};

static const int SENSORS_LIST_SIZE = (sizeof(SENSORS_LIST) / sizeof(SENSORS_LIST[0]));

static const sensors_event_t SENSOR_EVENT_FINGER_ON = {
    .version = sizeof(sensors_event_t),
    .sensor = SENSORS_LIST[0].handle,
    .type = SENSOR_TYPE_UDFPS_SENSOR,
    .timestamp = 0,
    .data = {0, 0, 0, 0},
};

sensors_poll_context_t::sensors_poll_context_t() {
    mEnabled = true;
    mPressed = false;

    IUdfpsSensor::getService()->setCallback(this);
}

int sensors_poll_context_t::activate(int /*handle*/, int enabled) {
    setEnabled(!!enabled);

    LOG(INFO) << "Enabled: " << mEnabled;

    return 0;
}

int sensors_poll_context_t::setDelay(int /*handle*/, int64_t /*ns*/) {
    return 0;
}

int sensors_poll_context_t::poll(sensors_event_t* data, int count) {
    bool enabled = false;
    bool pressed = false;
    LOG(INFO) << "poll called, count: " << count;

    // poll() must wait until the sensor is reactivated and a new event is available
    LOG(INFO) << "poll: waiting for finger press";
    while (!enabled && !pressed) {
        enabled = getEnabled();
        pressed = getPressed();
    }

    setPressed(false);
    setEnabled(false);

    LOG(INFO) << "Finger pressed";

    memcpy(data, &SENSOR_EVENT_FINGER_ON, sizeof(sensors_event_t));

    return 1;
}

Return<void> sensors_poll_context_t::onPressed() {
    setPressed(true);

    LOG(INFO) << "onPressed received";

    return Void();
}

bool sensors_poll_context_t::getEnabled() {
    std::lock_guard<std::mutex> lock(mEnabledMutex);
    return mEnabled;
}

void sensors_poll_context_t::setEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mEnabledMutex);
    mEnabled = enabled;
}

bool sensors_poll_context_t::getPressed() {
    std::lock_guard<std::mutex> lock(mPressedMutex);
    return mPressed;
}

void sensors_poll_context_t::setPressed(bool pressed) {
    std::lock_guard<std::mutex> lock(mPressedMutex);
    mPressed = pressed;
}

// HAL methods

static int close(struct hw_device_t *dev) {
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    if (ctx) {
        delete ctx;
    }
    return 0;
}

static int activate(struct sensors_poll_device_t *dev,
        int handle, int enabled) {
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    return ctx->activate(handle, enabled);
}

static int setDelay(struct sensors_poll_device_t *dev,
        int handle, int64_t ns) {
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    return ctx->setDelay(handle, ns);
}

static int poll(struct sensors_poll_device_t *dev,
        sensors_event_t* data, int count) {
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    return ctx->poll(data, count);
}

static int get_sensors_list(struct sensors_module_t */*module*/,
                            struct sensor_t const **list) {

    *list = SENSORS_LIST;
    return SENSORS_LIST_SIZE;
}

static int open(const struct hw_module_t *module, const char */*id*/, struct hw_device_t **device) {
    int status = -EINVAL;
    sensors_poll_context_t *dev = new sensors_poll_context_t();

    memset(&dev->device, 0, sizeof(sensors_poll_device_1));

    dev->device.common.tag = HARDWARE_DEVICE_TAG;
    dev->device.common.version = SENSORS_DEVICE_API_VERSION_1_3;
    dev->device.common.module = const_cast<hw_module_t *>(module);
    dev->device.common.close = close;
    dev->device.activate = activate;
    dev->device.setDelay = setDelay;
    dev->device.poll = poll;

    *device = &dev->device.common;
    status = 0;

    LOG(INFO) << "Opened sensor HAL";

    return status;
}

static struct hw_module_methods_t sensors_module_methods = {
    .open = open
};

extern "C" struct sensors_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .version_major = 1,
        .version_minor = 0,
        .id = SENSORS_HARDWARE_MODULE_ID,
        .name = "Virtual UDFPS sensor module",
        .author = "Sebastiano Barezzi, The LineageOS Project",
        .methods = &sensors_module_methods,
        .dso = NULL,
        .reserved = {0}
    },
    .get_sensors_list = get_sensors_list,
};
