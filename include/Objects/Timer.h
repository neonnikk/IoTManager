#pragma once

#include <Arduino.h>

class Timer {
   public:
    Timer(const String& name, unsigned long time);
    ~Timer();

    void setTime(unsigned long time);
    const String name() const;
    bool tick();
    void stop();

   private:
    void onTimer();

   private:
    bool _enabled;
    String _name;
    unsigned long _time;
};
