#pragma once

#include <Arduino.h>

#ifdef ESP8266
#include <Servo.h>
#else
#include <ESP32Servo.h>
#endif

#include "Base/Item.h"
#include "Base/Value.h"
#include "Base/ValueMap.h"

class BasicServo : public Item,
                   public PinAssigned,
                   public ValueMap {
   public:
    BasicServo(const String& name, const String& assign) : Item{name, assign},
                                                           PinAssigned{this},
                                                           ValueMap{VT_INT} {
        _obj.attach(getPin());
    };

    ~BasicServo() {
    }

    void onValueUpdate(const String& value) override {
        _obj.write(value.toInt());
    }

   private:
    Servo _obj;
};
