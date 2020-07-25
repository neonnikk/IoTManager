#pragma once

#include <Arduino.h>

#include "Base/KeyValue.h"

namespace Scenario {

void process(KeyValue*);
void process(const String);
void init();
void loop();
void enableBlock(size_t num, boolean enable);
bool isBlockEnabled(size_t num);

}  // namespace Scenario