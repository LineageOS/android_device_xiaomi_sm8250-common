package kona

import (
    "android/soong/android"
    "android/soong/cc"
    "strconv"
    "strings"
)

func biometricsFlags(ctx android.BaseContext) []string {
    var cflags []string

    var config = ctx.AConfig().VendorConfig("XIAOMI_KONA_BIOMETRICS")
    var halClasses = strings.Split(strings.TrimSpace(config.String("HAL_CLASSES")), ",")

    cflags = append(cflags, "-DHAL_CLASSES=\"" + strings.Join(halClasses, "\", \"") + "\"")
    cflags = append(cflags, " -DHAL_CLASSES_SIZE=" + strconv.Itoa(len(halClasses)))

    return cflags
}

func biometricsHalBinary(ctx android.LoadHookContext) {
    type props struct {
        Target struct {
            Android struct {
                Cflags []string
            }
        }
    }

    p := &props{}
    p.Target.Android.Cflags = biometricsFlags(ctx)
    ctx.AppendProperties(p)
}

func biometricsHalBinaryFactory() android.Module {
    module, _ := cc.NewBinary(android.HostAndDeviceSupported)
    newMod := module.Init()
    android.AddLoadHook(newMod, biometricsHalBinary)
    return newMod
}
