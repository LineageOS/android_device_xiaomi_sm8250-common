/*
 * Copyright (C) 2021 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <android-base/properties.h>
#include <libinit_utils.h>

#include <libinit_variant.h>

using android::base::GetProperty;

#define HWC_PROP "ro.boot.hwc"
#define SKU_PROP "ro.boot.product.hardware.sku"

void search_variant(const std::vector<variant_info_t> variants) {
    std::string hwc_value = GetProperty(HWC_PROP, "");
    std::string sku_value = GetProperty(SKU_PROP, "");

    for (const auto& variant : variants) {
        if ((variant.hwc_value == "" || variant.hwc_value == hwc_value) &&
            (variant.sku_value == "" || variant.sku_value == sku_value)) {
            set_variant_props(variant);
            break;
        }
    }
}

static const std::string[] kACDBFiles = {
    "Forte_Bluetooth_cal.acdb",
    "Forte_General_cal.acdb",
    "Forte_Global_cal.acdb",
    "Forte_Handset_cal.acdb",
    "Forte_Hdmi_cal.acdb",
    "Forte_Headset_cal.acdb",
    "Forte_Speaker_cal.acdb",
    "Forte_workspaceFile.qwsp",
};

void set_variant_props(const variant_info_t variant) {
    set_ro_build_prop("brand", variant.brand, true);
    set_ro_build_prop("device", variant.device, true);
    set_ro_build_prop("marketname", variant.marketname, true);
    set_ro_build_prop("model", variant.model, true);

    set_ro_build_prop("fingerprint", variant.build_fingerprint);
    property_override("ro.bootimage.build.fingerprint", variant.build_fingerprint);

    property_override("ro.build.description", fingerprint_to_description(variant.build_fingerprint));

    if (variant.nfc)
        property_override(SKU_PROP, "nfc");

    property_override("ro.arch", variant.device);

    for (int i = 0; i < kACDBFiles.size(); i++) {
        std::string acdb_path = "/vendor/etc/acdbdata/" + variant.device + "/" + kACDBFiles[i];
        property_override("persist.vendor.audio.calfile" + i, acdb_path);
    }
}
