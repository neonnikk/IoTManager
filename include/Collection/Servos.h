#pragma once

#include "Objects/BasicServo.h"

class Servos;
extern Servos servos;

class Servos {
   public:
    Servos();

    BasicServo* add(const String& name, const String& pin);

    BasicServo* get(const String& name);

    size_t count();

    BasicServo* last();

   private:
    std::vector<BasicServo*> _items;
};
