#pragma once

#include <Arduino.h>

class Writer {
   public:
    virtual ~Writer(){};
    virtual bool begin() = 0;
    virtual bool write(const String&, size_t length) = 0;
    virtual void end() = 0;
};
