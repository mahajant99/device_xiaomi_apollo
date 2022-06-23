/*
 * Copyright (C) 2021 Paranoid Android
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
#include <cstdlib>
#include <string.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <android-base/properties.h>

#include "property_service.h"
#include "vendor_init.h"

using android::base::GetProperty;
using std::string;

void property_override(char const prop[], char const value[], bool add = true)
{
    auto pi = (prop_info*) __system_property_find(prop);

    if (pi != nullptr)
        __system_property_update(pi, value, strlen(value));
    else if (add)
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void set_ro_build_prop(const string &source, const string &prop,
                       const string &value, bool product = false) {
    string prop_name;

    if (product)
        prop_name = "ro.product." + source + prop;
    else
        prop_name = "ro." + source + "build." + prop;

    property_override(prop_name.c_str(), value.c_str(), true);
}

void set_device_props(const string brand, const string marketname,
        const string device, const string model) {
    // list of partitions to override props
    string source_partitions[] = { "", "bootimage", "odm.", "product.",
                                   "system", "system_ext.", "vendor." };

    for (const string &source : source_partitions) {
        set_ro_build_prop(source, "brand", brand, true);
        set_ro_build_prop(source, "marketname", marketname, true);
        set_ro_build_prop(source, "device", device, true);
        set_ro_build_prop(source, "product", device, false);
        set_ro_build_prop(source, "model", model, true);
    }
}

void vendor_load_properties()
{
    /*
     * Detect device and configure properties
     */
    string hwc = GetProperty("ro.boot.hwc", "");
    string sku = GetProperty("ro.boot.product.hardware.sku", "");

    if (hwc == "CN") { // K30S Ultra (China)
            set_device_props("Xiaomi", "K30S Ultra", "apollo", "M2007J3SC");
    } else {
        if (sku == "pro") { // Mi 10T Pro
            set_device_props("Xiaomi", "Mi 10T Pro", "apollo", "M2007J3SG");
        } else { // Mi 10T
            set_device_props("Xiaomi", "Mi 10T", "apollo", "M2007J3SY");
        }
    }

    property_override("ro.build.fingerprint", "google/redfin/redfin:12/SQ3A.220605.009.A1/8643238:user/release-keys");
    property_override("ro.build.description", "apollo_global-user 12 RKQ1.211001.001 V13.0.3.0.SJDMIXM release-keys");
}
