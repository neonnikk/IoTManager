#pragma once

#include <Arduino.h>

namespace Pins {
void init();

bool isValid(const String& str);

void setPin(uint8_t num, uint8_t mode);

bool isBusy(uint8_t num);

};  // namespace Pins