#include "Objects/Timer.h"

#include "Global.h"

Timer::Timer(const char* name, unsigned long time) {
    _name = strdup(name);
    _time = time;
    runtime.write(name, true);
}

Timer::~Timer() {
    runtime.erase(_name);
    free(_name);
}

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

const char* Timer::name() const {
    return _name;
}

void Timer::onTimer() {
    runtime.write(_name, false);
}

void Timer::setTime(unsigned long value) {
    _time = value;
}

void Timer::stop() {
    _enabled = false;
}
