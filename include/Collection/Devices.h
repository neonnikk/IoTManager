#include <Arduino.h>

#pragma once

#include "Objects\DeviceItem.h"

namespace Devices {
void add(const String& deviceId, const String& name, const String& ip);
void get(String& result, unsigned long ttl_sec = 0);
void toCSV(const String filename);
const String asJson();
}  // namespace Devices
