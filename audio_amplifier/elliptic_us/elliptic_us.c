/*
 * Copyright (c) 2017-2018, 2021 The LineageOS Project
 * Copyright (c) 2017 Balázs Triszka <balika011@protonmail.ch>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "audio_amplifier.elliptic_us"

#include <errno.h>
#include <stdlib.h>
#include <cutils/log.h>
#include "audio_hw.h"
#include "platform_api.h"
#include <platform.h>

#define ULTRASOUND_CALIBRATION_FILE "/persist/audio/us_cal"
#define ULTRASOUND_CALIBRATION_MIXER "Ultrasound Calibration Data"

enum {
    ULTRASOUND_STATUS_DEFAULT,
    ULTRASOUND_STATUS_STARTED,
    ULTRASOUND_STATUS_STOPPED,
};

struct pcm_config pcm_config_us = {
    .channels = 1,
    .rate = 96000,
    .period_size = 1024,
    .period_count = 2,
    .format = PCM_FORMAT_S16_LE,
};

struct ultrasound_device {
    struct pcm *rx_pcm, *tx_pcm;
    int state;
    struct audio_device *adev;
};

static struct ultrasound_device *us = NULL;

void us_cal_load(void)
{
    FILE *f;
    char buff[5] = {0}, us_cal[64];
    struct mixer_ctl * ctl;
    int rc;

    f = fopen(ULTRASOUND_CALIBRATION_FILE, "r");
    if (!f) {
        ALOGE("%s: Cannot open calibration file: %s",
                __func__, ULTRASOUND_CALIBRATION_FILE);
        return;
    }

    for (size_t i = 0; i < sizeof(us_cal); i++) {
        fread(buff, 1, sizeof(buff), f);
        us_cal[i] = strtol(buff, 0, 16);
    }
    fclose(f);

    ctl = mixer_get_ctl_by_name(us->adev->mixer, ULTRASOUND_CALIBRATION_MIXER);
    if (!ctl) {
        ALOGE("%s: Could not get ctl for mixer cmd - %s",
                __func__, ULTRASOUND_CALIBRATION_MIXER);
        return;
    }

    rc = mixer_ctl_set_array(ctl, us_cal, sizeof(us_cal));
    if (rc < 0)
        ALOGE("%s: Could not set ctl, error:%d ", __func__, rc);
}

int us_init(struct audio_device *adev)
{
    ALOGD("%s: enter", __func__);

    if (us) {
        ALOGI("%s: ultrasound has been initialized!", __func__);
        return 0;
    }

    us = calloc(1, sizeof(struct ultrasound_device));
    if (!us) {
        ALOGE("%s: Out of memory!", __func__);
        return -ENOMEM;
    }

    us->adev = adev;

    us_cal_load();

    ALOGD("%s: exit, status(0)", __func__);

    return 0;
}

void us_deinit(void)
{
    ALOGD("%s: enter", __func__);

    if (us) {
        free(us);
        us = NULL;
    }

    ALOGD("%s: exit", __func__);
}

int stop_us(void)
{
    struct audio_usecase *rx_usecase, *tx_usecase;
    int rc = 0;

    ALOGD("%s: enter usecase: ultrasound", __func__);

    us->state = ULTRASOUND_STATUS_STOPPED;
    if (us->rx_pcm) {
        pcm_close(us->rx_pcm);
        us->rx_pcm = NULL;
    }

    if (us->tx_pcm) {
        pcm_close(us->tx_pcm);
        us->tx_pcm = NULL;
    }

    rx_usecase = get_usecase_from_list(us->adev, USECASE_AUDIO_ULTRASOUND_RX);
    if (!rx_usecase) {
        ALOGE("%s: Could not find the usecase (%d) in the list",
                __func__, USECASE_AUDIO_ULTRASOUND_RX);
        rc = -EINVAL;
    } else {
        disable_audio_route(us->adev, rx_usecase);
        disable_snd_device(us->adev, rx_usecase->out_snd_device);
        list_remove(&rx_usecase->list);
        free(rx_usecase);
    }

    tx_usecase = get_usecase_from_list(us->adev, USECASE_AUDIO_ULTRASOUND_TX);
    if (!rx_usecase) {
        ALOGE("%s: Could not find the usecase (%d) in the list",
                __func__, USECASE_AUDIO_ULTRASOUND_TX);
        rc = -EINVAL;
    } else {
        disable_audio_route(us->adev, tx_usecase);
        disable_snd_device(us->adev, tx_usecase->in_snd_device);
        list_remove(&tx_usecase->list);
        free(tx_usecase);
    }

    ALOGD("%s: exit: status(%d)", __func__, rc);

    return rc;
}

int us_start(void)
{
    int rx_device_id, tx_device_id;
    struct audio_usecase *rx_usecase, *tx_usecase;

    ALOGD("%s: enter", __func__);

    if (!us || us->state == ULTRASOUND_STATUS_STARTED)
        return -EPERM;

    ALOGD("%s: enter usecase: ultrasound", __func__);
    rx_device_id = platform_get_pcm_device_id(USECASE_AUDIO_ULTRASOUND_RX, PCM_PLAYBACK);
    tx_device_id = platform_get_pcm_device_id(USECASE_AUDIO_ULTRASOUND_TX, PCM_CAPTURE);
    if (rx_device_id < 0 || tx_device_id < 0) {
        ALOGE("%s: Invalid PCM devices (rx: %d tx: %d) for the usecase(ultrasound)",
                __func__, rx_device_id, tx_device_id);
        stop_us();
        ALOGE("%s: exit: status(%d)", __func__, -EIO);
        return -EIO;
    }

    rx_usecase = calloc(1, sizeof(struct audio_usecase));
    if (!rx_usecase) {
        ALOGE("%s: Out of memory!", __func__);
        return -ENOMEM;
    }

    rx_usecase->type = PCM_PLAYBACK;
    rx_usecase->out_snd_device = SND_DEVICE_OUT_ULTRASOUND_HANDSET;
    rx_usecase->id = USECASE_AUDIO_ULTRASOUND_RX;
    list_add_tail(&us->adev->usecase_list, &rx_usecase->list);

    enable_snd_device(us->adev, SND_DEVICE_OUT_ULTRASOUND_HANDSET);
    enable_audio_route(us->adev, rx_usecase);
    ALOGV("%s: Opening PCM playback device card_id(%d) device_id(%d)",
            __func__, us->adev->snd_card, rx_device_id);
    us->rx_pcm = pcm_open(us->adev->snd_card, rx_device_id, PCM_OUT, &pcm_config_us);
    if (us->rx_pcm && !pcm_is_ready(us->rx_pcm)) {
        ALOGE("%s: %s", __func__, pcm_get_error(us->rx_pcm));
        stop_us();
        ALOGE("%s: exit: status(%d)", __func__, -EIO);
        return -EIO;
    }

    tx_usecase = calloc(1, sizeof(struct audio_usecase));
    if (!tx_usecase) {
        ALOGE("%s: Out of memory!", __func__);
        return -ENOMEM;
    }

    tx_usecase->type = PCM_CAPTURE;
    tx_usecase->in_snd_device = SND_DEVICE_IN_ULTRASOUND_MIC;
    tx_usecase->id = USECASE_AUDIO_ULTRASOUND_TX;
    list_add_tail(&us->adev->usecase_list, &tx_usecase->list);

    enable_snd_device(us->adev, SND_DEVICE_IN_ULTRASOUND_MIC);
    enable_audio_route(us->adev, tx_usecase);
    ALOGV("%s: Opening PCM capture device card_id(%d) device_id(%d)",
            __func__, us->adev->snd_card, tx_device_id);
    us->tx_pcm = pcm_open(us->adev->snd_card, tx_device_id, PCM_IN, &pcm_config_us);
    if (us->tx_pcm && !pcm_is_ready(us->tx_pcm)) {
        ALOGD("%s: %s", __func__, pcm_get_error(us->tx_pcm));
        stop_us();
        ALOGE("%s: exit: status(%d)", __func__, -EIO);
        return -EIO;
    }

    pcm_start(us->rx_pcm);
    pcm_start(us->tx_pcm);
    us->state = ULTRASOUND_STATUS_STARTED;

    ALOGD("%s: exit, status(0)", __func__);

    return 0;
}

int us_stop(void)
{
    ALOGD("%s: enter", __func__);

    if (!us || us->state != ULTRASOUND_STATUS_STARTED)
        return -EPERM;

    stop_us();

    return 0;
}

static int amp_dev_close(hw_device_t* device) {
    us_t* dev = (us_t*)device;
    if (dev) free(dev);

    return 0;
}

static int amp_module_open(const hw_module_t* module, const char* name, hw_device_t** device) {
    if (strcmp(name, AMPLIFIER_HARDWARE_INTERFACE)) {
        ALOGE("%s:%d: %s does not match amplifier hardware interface name\n", __func__, __LINE__,
              name);
        return -ENODEV;
    }

    us_dev = calloc(1, sizeof(us_t));
    if (!us_dev) {
        ALOGE("%s:%d: Unable to allocate memory for amplifier device\n", __func__, __LINE__);
        return -ENOMEM;
    }

    us_dev->amp_dev.common.tag = HARDWARE_DEVICE_TAG;
    us_dev->amp_dev.common.module = (hw_module_t*)module;
    us_dev->amp_dev.common.version = HARDWARE_DEVICE_API_VERSION(1, 0);
    us_dev->amp_dev.common.close = amp_dev_close;

    us_dev->amp_dev.set_input_devices = NULL;
    us_dev->amp_dev.set_output_devices = NULL;
    us_dev->amp_dev.enable_output_devices = NULL;
    us_dev->amp_dev.enable_input_devices = NULL;
    us_dev->amp_dev.set_mode = NULL;
    us_dev->amp_dev.output_stream_start = NULL;
    us_dev->amp_dev.input_stream_start = NULL;
    us_dev->amp_dev.output_stream_standby = NULL;
    us_dev->amp_dev.input_stream_standby = NULL;
    us_dev->amp_dev.set_parameters = NULL;
    us_dev->amp_dev.out_set_parameters = NULL;
    us_dev->amp_dev.in_set_parameters = NULL;
    us_dev->amp_dev.set_feedback = amp_set_feedback;

    *device = (hw_device_t*)us_dev;

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
        .name = "Elliptic Ultrasound audio amplifier HAL",
        .author = "The LineageOS Project",
        .methods = &hal_module_methods,
    },
};
