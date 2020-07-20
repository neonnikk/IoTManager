#pragma once

#include <Arduino.h>
#include <functional>

#include "Config.h"

#include "Base/Item.h"
#include "Base/Assigned.h"
#include "Base/Value.h"

enum ButtonType_t {
    BUTTON_VIRTUAL,
    BUTTON_GPIO,
    BUTTON_SCEN,
    BUTTON_SCEN_LINE
};

class Button : public Item,
               public Value {
   public:
    Button(const String& name, const String& assign) : Item{name, assign},
                                                       Value{VT_INT},
                                                       _inverted{false} {};
    ~Button() {}

    void setInverted(bool inverted) {
        _inverted = inverted;
    }

    bool isInverted() {
        return _inverted;
    }

    const String toggleState() {
        bool state = getValue().toInt();
        setValue(String(!state));
        return getValue();
    }

   protected:
    bool _inverted;
};

class VirtualButton : public Button {
   public:
    VirtualButton(const String& name, const String& assign) : Button{name, assign} {};

    ~VirtualButton(){};
};

class ScenButton : public Button {
   public:
    ScenButton(const String& name, const String& assign) : Button{name, assign} {};

    ~ScenButton(){};

   protected:
    void onValueUpdate(const String& value) override {
        bool state = value.toInt();
        config.general()->enableScenario(state);
    }
};

class PinButton : public Button,
                  public PinAssigned {
   public:
    PinButton(const String& name, const String& assign) : Button{name, assign},
                                                          PinAssigned{this} {
        pinMode(getPin(), OUTPUT);
    };

    ~PinButton(){};

   protected:
    void onValueUpdate(const String& value) override {
        uint8_t pin = getPin();
        uint8_t state = isInverted() ? value.toInt() : !value.toInt();
        digitalWrite(pin, state);
        // Serial.printf("digitalWrite %d,%d\n", pin, state);
    }
};
