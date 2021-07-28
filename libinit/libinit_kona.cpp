/*
 * Copyright (C) 2021 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <android-base/properties.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <sys/sysinfo.h>

#include <libinit_kona.h>

using android::base::GetProperty;

#define HWC_PROP "ro.boot.hwc"
#define SKU_PROP "ro.boot.product.hardware.sku"

#define HEAPSTARTSIZE_PROP "dalvik.vm.heapstartsize"
#define HEAPGROWTHLIMIT_PROP "dalvik.vm.heapgrowthlimit"
#define HEAPSIZE_PROP "dalvik.vm.heapsize"
#define HEAPTARGETUTILIZATION_PROP "dalvik.vm.heaptargetutilization"
#define HEAPMINFREE_PROP "dalvik.vm.heapminfree"
#define HEAPMAXFREE_PROP "dalvik.vm.heapmaxfree"

#define TO_GB(b) (b * 1024ull * 1024 * 1024)

static const dalvik_heap_info_t dalvik_heap_info_6144 {
    .heapstartsize = "16m",
    .heapgrowthlimit = "256m",
    .heapsize = "512m",
    .heaptargetutilization = "0.5",
    .heapminfree = "8m",
    .heapmaxfree = "32m",
};

static const dalvik_heap_info_t dalvik_heap_info_4096 {
    .heapstartsize = "8m",
    .heapgrowthlimit = "256m",
    .heapsize = "512m",
    .heaptargetutilization = "0.6",
    .heapminfree = "8m",
    .heapmaxfree = "16m",
};

static const dalvik_heap_info_t dalvik_heap_info_2048 {
    .heapstartsize = "8m",
    .heapgrowthlimit = "192m",
    .heapsize = "512m",
    .heaptargetutilization = "0.75",
    .heapminfree = "512k",
    .heapmaxfree = "8m",
};

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

void set_variant_props(const variant_info_t variant) {
    set_ro_build_prop("brand", variant.brand, true);
    set_ro_build_prop("device", variant.device, true);
    set_ro_build_prop("marketname", variant.marketname, true);
    set_ro_build_prop("model", variant.model, true);

    set_ro_build_prop("fingerprint", variant.build_fingerprint);
    property_override("ro.bootimage.build.fingerprint", variant.build_fingerprint);
    property_override("ro.build.description", variant.build_description);

    if (variant.nfc)
        property_override(SKU_PROP, "nfc");
}

void set_dalvik_heap() {
    struct sysinfo sys;
    const dalvik_heap_info_t *dhi;

    sysinfo(&sys);

    if (sys.totalram > TO_GB(5))
        dhi = &dalvik_heap_info_6144;
    else if (sys.totalram > TO_GB(3))
        dhi = &dalvik_heap_info_4096;
    else
        dhi = &dalvik_heap_info_2048;

    property_override(HEAPSTARTSIZE_PROP, dhi->heapstartsize);
    property_override(HEAPGROWTHLIMIT_PROP, dhi->heapgrowthlimit);
    property_override(HEAPSIZE_PROP, dhi->heapsize);
    property_override(HEAPTARGETUTILIZATION_PROP, dhi->heaptargetutilization);
    property_override(HEAPMINFREE_PROP, dhi->heapminfree);
    property_override(HEAPMAXFREE_PROP, dhi->heapmaxfree);
}

void property_override(std::string prop, std::string value, bool add) {
    auto pi = (prop_info *) __system_property_find(prop.c_str());
    if (pi != nullptr) {
        __system_property_update(pi, value.c_str(), value.length());
    } else if (add) {
        __system_property_add(prop.c_str(), prop.length(), value.c_str(), value.length());
    }
}

std::vector<std::string> ro_props_default_source_order = {
    "odm.",
    "product.",
    "system.",
    "system_ext.",
    "vendor.",
    "",
};

void set_ro_build_prop(const std::string &prop, const std::string &value, bool product) {
    std::string prop_name;

    for (const auto &source : ro_props_default_source_order) {
        if (product)
            prop_name = "ro.product." + source + prop;
        else
            prop_name = "ro." + source + "build." + prop;

        property_override(prop_name, value, true);
    }
}
