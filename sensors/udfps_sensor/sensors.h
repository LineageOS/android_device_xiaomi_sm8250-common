/*
 * Copyright (C) 2022 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <hardware/hardware.h>
#include <hardware/sensors.h>
#include <mutex>
#include <vendor/lineage/biometrics/fingerprint/udfpssensor/1.0/IUdfpsSensor.h>

#define SENSOR_TYPE_UDFPS_SENSOR (SENSOR_TYPE_DEVICE_PRIVATE_BASE + 1)

using android::hardware::Return;
using android::hardware::Void;
using android::sp;
using vendor::lineage::biometrics::fingerprint::udfpssensor::V1_0::IUdfpsSensor;
using vendor::lineage::biometrics::fingerprint::udfpssensor::V1_0::IUdfpsSensorCallback;

class sensors_poll_context_t : public IUdfpsSensorCallback {
public:
	sensors_poll_device_1_t device;

	sensors_poll_context_t();

	int activate(int handle, int enabled);
	int setDelay(int handle, int64_t ns);
	int poll(sensors_event_t *data, int count);

	Return<void> onPressed() override;

private:
	bool mEnabled;
	std::mutex mEnabledMutex;
	bool getEnabled();
	void setEnabled(bool enabled);
	bool mPressed;
	std::mutex mPressedMutex;
	bool getPressed();
	void setPressed(bool pressed);
};
