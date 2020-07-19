#pragma once

#include <Arduino.h>

#ifdef ESP8266
#include <Servo.h>
#else
#include <ESP32Servo.h>
#endif

#include "Base/Item.h"

class BasicServo : public Item,
                  public PinAssigned,
                  public Value,
                  public ValueMap {
   public:
    BasicServo(const String& name, const String& assign) : Item{name, assign},
                                                          PinAssigned{this},
                                                          Value{VT_INT},
                                                          ValueMap{this} {
        _obj = new Servo();
        _obj->attach(getPin());
    };

    ~BasicServo() {
        delete _obj;
    }

    void onValueUpdate(const String& value) override {
        _obj->write(value.toInt());
    }

   private:
    Servo* _obj;
};
