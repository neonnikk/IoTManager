#include "Objects/Timer.h"

#include "Global.h"

Timer::Timer(const String& name, unsigned long time) : _name{name}, _time{time} {}

Timer::~Timer() {}

bool Timer::tick() {
    if (!_enabled) {
        return false;
    }
    if (!_time--) {
        onTimer();
        return true;
    };
    return false;
}

const String Timer::name() const {
    return _name;
}

void Timer::onTimer() {
}

void Timer::setTime(unsigned long value) {
    _time = value;
}

void Timer::stop() {
    _enabled = false;
}
