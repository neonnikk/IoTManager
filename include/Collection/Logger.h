#pragma once

#include <Arduino.h>

#include "Objects/LoggerTask.h"

typedef std::function<bool(LoggerTask*)> LoggerTaskHandler;

namespace Logger {
void init();
void add(size_t id, const String& name, unsigned long interval, unsigned long period);
void update();
void clear();
const String asJson();
void asCSV(const String& filename);
void forEach(LoggerTaskHandler func);
};  // namespace Logger
