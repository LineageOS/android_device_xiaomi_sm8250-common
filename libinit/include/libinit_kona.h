/*
 * Copyright (C) 2021 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBINIT_KONA_H
#define LIBINIT_KONA_H

#include <string>
#include <vector>

typedef struct variant_info {
    std::string hwc_value;
    std::string sku_value;

    std::string brand;
    std::string device;
    std::string marketname;
    std::string model;
    std::string build_description;
    std::string build_fingerprint;

    bool nfc;
} variant_info_t;

void search_variant(const std::vector<variant_info_t> variants);
void set_variant_props(const variant_info_t variant);

void property_override(char const prop[], char const value[], bool add = true);
void set_ro_build_prop(const std::string &prop, const std::string &value, bool product = false);

#endif // LIBINIT_KONA_H
