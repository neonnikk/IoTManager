#pragma once

#include <Arduino.h>
#include <functional>

#include <Bounce2.h>

#include "Base/Item.h"
#include "Base/Value.h"
#include "Base/Assigned.h"

class Switch;

typedef std::function<void(Switch*)> OnSwitchChangeState;

class Switch : public Item,
               public Value,
               public PinAssigned {
   public:
    Switch(const String& name, const String& assign) : Item{name, assign},
                                                       Value{VT_INT},
                                                       PinAssigned{this} {
        _obj.attach(getPin(), INPUT);
    }

    ~Switch() {
    }

    void setDebounce(int value) {
        _obj.interval(value);
    }

    const bool hasValue() override {
        return _obj.update();
    }

    const String onGetValue() override {
        return String(_obj.read(), DEC);
    }

   protected:
    Bounce _obj;
};
