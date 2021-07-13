/*
 * Copyright (C) 2015 The CyanogenMod Project
 * Copyright (C) 2020-2021 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "audio_amplifier.tfa98xx"
//#define LOG_NDEBUG 0

#include <cutils/str_parms.h>
#include <hardware/audio_amplifier.h>
#include <hardware/hardware.h>
#include <log/log.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

/* clang-format off */
#include "audio_hw.h"
#include "platform.h"
#include "platform_api.h"
/* clang-format on */

#define UNUSED __attribute__((unused))

typedef struct amp_device {
    amplifier_device_t amp_dev;
    struct audio_device* adev;
    struct audio_usecase* usecase_tx;
    struct pcm* tfa98xx_out;
} tfa_t;

static tfa_t* tfa_dev = NULL;

static int is_speaker(uint32_t snd_device) {
    int speaker = 0;
    switch (snd_device) {
        case SND_DEVICE_OUT_SPEAKER:
        case SND_DEVICE_OUT_SPEAKER_REVERSE:
        case SND_DEVICE_OUT_SPEAKER_AND_HEADPHONES:
        case SND_DEVICE_OUT_VOICE_SPEAKER:
        case SND_DEVICE_OUT_VOICE_SPEAKER_2:
        case SND_DEVICE_OUT_SPEAKER_AND_HDMI:
        case SND_DEVICE_OUT_SPEAKER_AND_USB_HEADSET:
        case SND_DEVICE_OUT_SPEAKER_AND_ANC_HEADSET:
            speaker = 1;
            break;
    }

    return speaker;
}

int tfa98xx_start_feedback(void* adev, uint32_t snd_device) {
    tfa_dev->adev = (struct audio_device*)adev;
    int pcm_dev_tx_id = 0, rc = 0;
    struct pcm_config pcm_config_tfa98xx = {
            .channels = 2,
            .rate = 48000,
            .period_size = 256,
            .period_count = 4,
            .format = PCM_FORMAT_S16_LE,
            .start_threshold = 0,
            .stop_threshold = INT_MAX,
            .silence_threshold = 0,
    };

    if (!tfa_dev) {
        ALOGE("%d: Invalid params", __LINE__);
        return -EINVAL;
    }

    if (tfa_dev->tfa98xx_out || !is_speaker(snd_device)) return 0;

    tfa_dev->usecase_tx = (struct audio_usecase*)calloc(1, sizeof(struct audio_usecase));
    if (!tfa_dev->usecase_tx) {
        ALOGE("%d: failed to allocate usecase", __LINE__);
        return -ENOMEM;
    }
    tfa_dev->usecase_tx->id = USECASE_AUDIO_SPKR_CALIB_TX;
    tfa_dev->usecase_tx->type = PCM_CAPTURE;
    tfa_dev->usecase_tx->in_snd_device = 232;
    list_init(&tfa_dev->usecase_tx->device_list);

    list_add_tail(&tfa_dev->adev->usecase_list, &tfa_dev->usecase_tx->list);
    enable_snd_device(tfa_dev->adev, tfa_dev->usecase_tx->in_snd_device);
    enable_audio_route(tfa_dev->adev, tfa_dev->usecase_tx);

    pcm_dev_tx_id = platform_get_pcm_device_id(tfa_dev->usecase_tx->id, tfa_dev->usecase_tx->type);
    ALOGD("pcm_dev_tx_id = %d", pcm_dev_tx_id);
    if (pcm_dev_tx_id < 0) {
        ALOGE("%d: Invalid pcm device for usecase (%d)", __LINE__, tfa_dev->usecase_tx->id);
        rc = -ENODEV;
        goto error;
    }

    tfa_dev->tfa98xx_out =
            pcm_open(tfa_dev->adev->snd_card, pcm_dev_tx_id, PCM_IN, &pcm_config_tfa98xx);
    if (!(tfa_dev->tfa98xx_out || pcm_is_ready(tfa_dev->tfa98xx_out))) {
        ALOGE("%d: %s", __LINE__, pcm_get_error(tfa_dev->tfa98xx_out));
        rc = -EIO;
        goto error;
    }

    rc = pcm_start(tfa_dev->tfa98xx_out);
    if (rc < 0) {
        ALOGE("%d: pcm start for TX failed", __LINE__);
        rc = -EINVAL;
        goto error;
    }
    return 0;

error:
    ALOGE("%s: error case", __func__);
    if (tfa_dev->tfa98xx_out != 0) {
        pcm_close(tfa_dev->tfa98xx_out);
        tfa_dev->tfa98xx_out = NULL;
    }
    list_remove(&tfa_dev->usecase_tx->list);
    disable_snd_device(tfa_dev->adev, tfa_dev->usecase_tx->in_snd_device);
    disable_audio_route(tfa_dev->adev, tfa_dev->usecase_tx);
    free(tfa_dev->usecase_tx);

    return rc;
}

void tfa98xx_stop_feedback(void* adev, uint32_t snd_device) {
    tfa_dev->adev = (struct audio_device*)adev;
    if (!tfa_dev) {
        ALOGE("%s: Invalid params", __func__);
        return;
    }

    if (!is_speaker(snd_device)) return;

    if (tfa_dev->tfa98xx_out) {
        pcm_close(tfa_dev->tfa98xx_out);
        tfa_dev->tfa98xx_out = NULL;
    }

    disable_snd_device(tfa_dev->adev, SND_DEVICE_IN_CAPTURE_VI_FEEDBACK);

    tfa_dev->usecase_tx = get_usecase_from_list(tfa_dev->adev, USECASE_AUDIO_SPKR_CALIB_TX);
    if (tfa_dev->usecase_tx) {
        list_remove(&tfa_dev->usecase_tx->list);
        disable_audio_route(tfa_dev->adev, tfa_dev->usecase_tx);
        free(tfa_dev->usecase_tx);
    }
    return;
}

static int amp_set_feedback(UNUSED amplifier_device_t* device, void* adev, uint32_t devices,
                            bool enable) {
    tfa_dev->adev = (struct audio_device*)adev;
    if (enable) {
        tfa98xx_start_feedback(tfa_dev->adev, devices);
    } else {
        tfa98xx_stop_feedback(tfa_dev->adev, devices);
    }
    return 0;
}

static int amp_dev_close(hw_device_t* device) {
    tfa_t* dev = (tfa_t*)device;
    if (dev) free(dev);

    return 0;
}

static int amp_module_open(const hw_module_t* module, const char* name, hw_device_t** device) {
    if (strcmp(name, AMPLIFIER_HARDWARE_INTERFACE)) {
        ALOGE("%s:%d: %s does not match amplifier hardware interface name\n", __func__, __LINE__,
              name);
        return -ENODEV;
    }

    tfa_dev = calloc(1, sizeof(tfa_t));
    if (!tfa_dev) {
        ALOGE("%s:%d: Unable to allocate memory for amplifier device\n", __func__, __LINE__);
        return -ENOMEM;
    }

    tfa_dev->amp_dev.common.tag = HARDWARE_DEVICE_TAG;
    tfa_dev->amp_dev.common.module = (hw_module_t*)module;
    tfa_dev->amp_dev.common.version = HARDWARE_DEVICE_API_VERSION(1, 0);
    tfa_dev->amp_dev.common.close = amp_dev_close;

    tfa_dev->amp_dev.set_input_devices = NULL;
    tfa_dev->amp_dev.set_output_devices = NULL;
    tfa_dev->amp_dev.enable_output_devices = NULL;
    tfa_dev->amp_dev.enable_input_devices = NULL;
    tfa_dev->amp_dev.set_mode = NULL;
    tfa_dev->amp_dev.output_stream_start = NULL;
    tfa_dev->amp_dev.input_stream_start = NULL;
    tfa_dev->amp_dev.output_stream_standby = NULL;
    tfa_dev->amp_dev.input_stream_standby = NULL;
    tfa_dev->amp_dev.set_parameters = NULL;
    tfa_dev->amp_dev.out_set_parameters = NULL;
    tfa_dev->amp_dev.in_set_parameters = NULL;
    tfa_dev->amp_dev.set_feedback = amp_set_feedback;

    *device = (hw_device_t*)tfa_dev;

    return 0;
}

static struct hw_module_methods_t hal_module_methods = {
        .open = amp_module_open,
};

/* clang-format off */
amplifier_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = AMPLIFIER_MODULE_API_VERSION_0_1,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = AMPLIFIER_HARDWARE_MODULE_ID,
        .name = "TFA98XX audio amplifier HAL",
        .author = "The LineageOS Project",
        .methods = &hal_module_methods,
    },
};
