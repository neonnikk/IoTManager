#include "Pins.h"

#include "Config.h"
#include "Utils/StringUtils.h"

#define NUM_PINS 17

namespace Pins {
bool _used[NUM_PINS];

void init() {
    memset(&_used[0], 0, NUM_PINS);
}

bool isValid(const String& str) {
    return isDigitStr(str) && (str.toInt() > 0) && (str.toInt() < NUM_PINS);
}

void setPin(uint8_t num, uint8_t mode) {
    _used[num] = true;
    pinMode(num, mode);
}

bool isBusy(uint8_t num) {
    return _used[num];
}

};  // namespace Pins