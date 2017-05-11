/*
 * Copyright (C) 2021 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "audio_amplifier.multihal"

#include <hardware/audio_amplifier.h>
#include <log/log.h>
#include <stdlib.h>

enum amp_devices_enum {
    ELLIPTIC_US,
    TFA98XX,
    MAX_AMPLIFIER_DEVICES,
};

static const char *amp_devices_list[MAX_AMPLIFIER_DEVICES] = {
    "elliptic_us",
    "tfa98xx",
};

static amplifier_device_t *amp_devices[MAX_AMPLIFIER_DEVICES];

#define UNUSED __attribute__((unused))

// Load all the HALs
static void amplifier_open_devices() {
    int rc;
    amplifier_module_t* module;
    unsigned int i;

    for (i = 0; i < MAX_AMPLIFIER_DEVICES; i++) {
        rc = hw_get_module_by_class(AMPLIFIER_HARDWARE_MODULE_ID, amp_devices_list[i],
                    (const hw_module_t**)&module);
        if (rc) {
            ALOGE("%s: Failed to obtain reference to amplifier module, class %s: %s\n",
                    __func__, amp_devices_list[i], strerror(-rc));
            continue;
        }

        rc = amplifier_device_open((const hw_module_t*)module, &amp_devices[i]);
        if (rc) {
            ALOGE("%s: Failed to open amplifier hardware device, class %s: %s\n",
                    __func__, amp_devices_list[i], strerror(-rc));
            amp_devices[i] = NULL;
            continue;
        }

        ALOGI("%s: Successfully opened amplifier hardware device, class %s\n",
                __func__, amp_devices_list[i]);
    }

    return;
}

// Device methods

static int amplifier_set_input_devices(UNUSED amplifier_device_t *device,
        uint32_t devices) {
    unsigned int i;
    for (i = 0; i < MAX_AMPLIFIER_DEVICES; i++) {
        if (amp_devices[i] && amp_devices[i]->set_input_devices)
            return amp_devices[i]->set_input_devices(amp_devices[i], devices);
    }

    return 0;
}

static int amplifier_set_output_devices(UNUSED amplifier_device_t *device,
        uint32_t devices) {
    unsigned int i;
    for (i = 0; i < MAX_AMPLIFIER_DEVICES; i++) {
        if (amp_devices[i] && amp_devices[i]->set_output_devices)
            return amp_devices[i]->set_output_devices(amp_devices[i], devices);
    }

    return 0;
}

static int amplifier_enable_input_devices(UNUSED amplifier_device_t *device,
        uint32_t devices, bool enable) {
    unsigned int i;
    for (i = 0; i < MAX_AMPLIFIER_DEVICES; i++) {
        if (amp_devices[i] && amp_devices[i]->enable_input_devices)
            return amp_devices[i]->enable_input_devices(amp_devices[i], devices, enable);
    }

    return 0;
}

static int amplifier_enable_output_devices(UNUSED amplifier_device_t *device,
        uint32_t devices, bool enable) {
    unsigned int i;
    for (i = 0; i < MAX_AMPLIFIER_DEVICES; i++) {
        if (amp_devices[i] && amp_devices[i]->enable_output_devices)
            return amp_devices[i]->enable_output_devices(amp_devices[i], devices, enable);
    }

    return 0;
}

static int amplifier_set_mode(UNUSED amplifier_device_t *device,
        audio_mode_t mode) {
    unsigned int i;
    for (i = 0; i < MAX_AMPLIFIER_DEVICES; i++) {
        if (amp_devices[i] && amp_devices[i]->set_mode)
            return amp_devices[i]->set_mode(amp_devices[i], mode);
    }

    return 0;
}

static int amplifier_output_stream_start(UNUSED amplifier_device_t *device,
        struct audio_stream_out* stream, bool offload) {
    unsigned int i;
    for (i = 0; i < MAX_AMPLIFIER_DEVICES; i++) {
        if (amp_devices[i] && amp_devices[i]->output_stream_start)
            return amp_devices[i]->output_stream_start(amp_devices[i], stream, offload);
    }

    return 0;
}

static int amplifier_input_stream_start(UNUSED amplifier_device_t *device,
        struct audio_stream_in* stream) {
    unsigned int i;
    for (i = 0; i < MAX_AMPLIFIER_DEVICES; i++) {
        if (amp_devices[i] && amp_devices[i]->input_stream_start)
            return amp_devices[i]->input_stream_start(amp_devices[i], stream);
    }

    return 0;
}

static int amplifier_output_stream_standby(UNUSED amplifier_device_t *device,
        struct audio_stream_out* stream) {
    unsigned int i;
    for (i = 0; i < MAX_AMPLIFIER_DEVICES; i++) {
        if (amp_devices[i] && amp_devices[i]->output_stream_standby)
            return amp_devices[i]->output_stream_standby(amp_devices[i], stream);
    }

    return 0;
}

static int amplifier_input_stream_standby(UNUSED amplifier_device_t *device,
        struct audio_stream_in* stream) {
    unsigned int i;
    for (i = 0; i < MAX_AMPLIFIER_DEVICES; i++) {
        if (amp_devices[i] && amp_devices[i]->input_stream_standby)
            return amp_devices[i]->input_stream_standby(amp_devices[i], stream);
    }

    return 0;
}

static int amplifier_set_parameters(UNUSED amplifier_device_t *device,
        struct str_parms* parms) {
    unsigned int i;
    for (i = 0; i < MAX_AMPLIFIER_DEVICES; i++) {
        if (amp_devices[i] && amp_devices[i]->set_parameters)
            return amp_devices[i]->set_parameters(amp_devices[i], parms);
    }

    return 0;
}

static int amplifier_out_set_parameters(UNUSED amplifier_device_t *device,
        struct str_parms* parms) {
    unsigned int i;
    for (i = 0; i < MAX_AMPLIFIER_DEVICES; i++) {
        if (amp_devices[i] && amp_devices[i]->out_set_parameters)
            return amp_devices[i]->out_set_parameters(amp_devices[i], parms);
    }

    return 0;
}

static int amplifier_in_set_parameters(UNUSED amplifier_device_t *device,
        struct str_parms* parms) {
    unsigned int i;
    for (i = 0; i < MAX_AMPLIFIER_DEVICES; i++) {
        if (amp_devices[i] && amp_devices[i]->in_set_parameters)
            return amp_devices[i]->in_set_parameters(amp_devices[i], parms);
    }

    return 0;
}

static int amplifier_set_feedback(UNUSED amplifier_device_t *device,
        void *adev, uint32_t devices, bool enable) {
    unsigned int i;
    for (i = 0; i < MAX_AMPLIFIER_DEVICES; i++) {
        if (amp_devices[i] && amp_devices[i]->set_feedback)
            return amp_devices[i]->set_feedback(amp_devices[i], adev, devices, enable);
    }

    return 0;
}

static int amplifier_close(hw_device_t* device) {
    unsigned int i;
    for (i = 0; i < MAX_AMPLIFIER_DEVICES; i++) {
        if (amp_devices[i])
            amplifier_device_close(amp_devices[i]);
        amp_devices[i] = NULL;
    }

    if (device)
        free(device);

    return 0;
}

// HAL methods

static int amplifier_open(const hw_module_t *module, const char *name,
        hw_device_t **device) {
    if (strcmp(name, AMPLIFIER_HARDWARE_INTERFACE)) {
        ALOGE("%s:%d: %s does not match amplifier hardware interface name\n",
                __func__, __LINE__, name);
        return -ENODEV;
    }

    amplifier_device_t *amp_dev = calloc(1, sizeof(amplifier_device_t));
    if (!amp_dev) {
        ALOGE("%s:%d: Unable to allocate memory for amplifier device\n",
                __func__, __LINE__);
        return -ENOMEM;
    }

    amp_dev->common.tag = HARDWARE_DEVICE_TAG;
    amp_dev->common.module = (hw_module_t *) module;
    amp_dev->common.version = HARDWARE_DEVICE_API_VERSION(1, 0);
    amp_dev->common.close = amplifier_close;

    amp_dev->set_input_devices = amplifier_set_input_devices;
    amp_dev->set_output_devices = amplifier_set_output_devices;
    amp_dev->enable_output_devices = amplifier_enable_output_devices;
    amp_dev->enable_input_devices = amplifier_enable_input_devices;
    amp_dev->set_mode = amplifier_set_mode;
    amp_dev->output_stream_start = amplifier_output_stream_start;
    amp_dev->input_stream_start = amplifier_input_stream_start;
    amp_dev->output_stream_standby = amplifier_output_stream_standby;
    amp_dev->input_stream_standby = amplifier_input_stream_standby;
    amp_dev->set_parameters = amplifier_set_parameters;
    amp_dev->out_set_parameters = amplifier_out_set_parameters;
    amp_dev->in_set_parameters = amplifier_in_set_parameters;
    amp_dev->set_feedback = amplifier_set_feedback;

    *device = (hw_device_t *) amp_dev;

    amplifier_open_devices();

    return 0;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = amplifier_open,
};

amplifier_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = AMPLIFIER_MODULE_API_VERSION_0_1,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = AMPLIFIER_HARDWARE_MODULE_ID,
        .name = "MultiHAL audio amplifier HAL",
        .author = "The LineageOS Project",
        .methods = &hal_module_methods,
    },
};
