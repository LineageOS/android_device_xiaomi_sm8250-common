/*
 * Copyright (C) 2018 The LineageOS Project
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

import android.os.Bundle;
import android.view.MenuItem;
import android.content.Context;
import android.content.SharedPreferences;
import android.provider.Settings;
import androidx.preference.Preference;
import androidx.preference.Preference.OnPreferenceChangeListener;
import androidx.preference.PreferenceFragment;
import androidx.preference.SwitchPreference;

import org.lineageos.settings.R;
import org.lineageos.settings.touchsampling.TouchSamplingUtils;
import org.lineageos.settings.utils.FileUtils;

public class TouchSamplingSettingsFragment extends PreferenceFragment implements
        OnPreferenceChangeListener {

    private static final String HTSR_ENABLE_KEY = "htsr_enable";
    public static final String SHAREDHTSR = "SHAREDHTSR";

    private SwitchPreference mHTSRPreference;

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
      addPreferencesFromResource(R.xml.htsr_settings);
        getActivity().getActionBar().setDisplayHomeAsUpEnabled(true);
        mHTSRPreference = (SwitchPreference) findPreference(HTSR_ENABLE_KEY);
        mHTSRPreference.setEnabled(true);
        mHTSRPreference.setOnPreferenceChangeListener(this);
        enableHTSR(0);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        if (HTSR_ENABLE_KEY.equals(preference.getKey())) {
            enableHTSR((Boolean) newValue ? 1 : 0);
        }
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            getActivity().onBackPressed();
            return true;
        }
        return false;
    }

    private void enableHTSR(Integer enable) {
            FileUtils.writeLine(TouchSamplingUtils.HTSR_FILE, enable.toString());
            SharedPreferences preferences = getActivity().getSharedPreferences(SHAREDHTSR,Context.MODE_PRIVATE);
            SharedPreferences.Editor editor = preferences.edit();
            editor.putInt(SHAREDHTSR, enable);
            editor.commit();
    }
}
