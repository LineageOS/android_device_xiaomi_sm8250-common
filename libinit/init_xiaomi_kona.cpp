/*
 * Copyright (C) 2021 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <libinit_dalvik_heap.h>

#include "vendor_init.h"

void vendor_load_properties() {
    set_dalvik_heap();
}
