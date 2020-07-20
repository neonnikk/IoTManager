#pragma once

#include <Arduino.h>

const String getDeviceId();

const String getUniqueId(const String& name);

const String getMemoryStatus();

const String getHeapStats();

const String getMacAddress();

