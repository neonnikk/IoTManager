#pragma once

#include <Arduino.h>

class Runner {
   public:
    virtual void run(const char *, Print *);
};
