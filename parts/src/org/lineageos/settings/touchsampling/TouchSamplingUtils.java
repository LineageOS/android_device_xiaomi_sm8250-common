/*
 * Copyright (C) 2015 The CyanogenMod Project
 *               2017-2019 The LineageOS Project
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

package org.lineageos.settings.touchsampling;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.SharedPreferences;
import android.os.UserHandle;
import android.provider.Settings;
import android.util.Log;
import androidx.preference.PreferenceManager;

import org.lineageos.settings.utils.FileUtils;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.BufferedReader;
import java.io.FileReader;

public final class TouchSamplingUtils {

    private static final String TAG = "TouchSamplingUtils";
    public static final String HTSR_FILE = "/sys/devices/virtual/touch/touch_dev/bump_sample_rate";

    public static void restoreSamplingValue(Context context) {
        SharedPreferences sharedPref = context.getSharedPreferences(TouchSamplingSettingsFragment.SHAREDHTSR, Context.MODE_PRIVATE);
        Integer htsrstate = sharedPref.getInt(TouchSamplingSettingsFragment.SHAREDHTSR, 0);
        FileUtils.writeLine(HTSR_FILE, htsrstate.toString());
    }
}
