package kona

import (
    "android/soong/android"
)

func init() {
    android.RegisterModuleType("xiaomi_kona_fod_hal_binary", fodHalBinaryFactory)
}
