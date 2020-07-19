#pragma once

#include "Objects/Switch.h"

class Switches;

extern Switches switches;

class Switches {
   public:
    Switch* add(const String& name, const String& assign);
    void loop();
    Switch* last();
    Switch* at(size_t index);
    Switch* get(const String name);
    size_t count();

    void setOnChangeState(OnSwitchChangeState);

   private:
    std::vector<Switch*> _items;
    OnSwitchChangeState _onChange;
};
