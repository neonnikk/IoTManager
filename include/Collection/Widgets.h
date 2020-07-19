#pragma once

#include <Arduino.h>
#include <functional>

typedef std::function<void(String)> JsonHandler;

namespace Widgets {
void createWidget(String& descr, String& page, const String& order, const String& templateName, const String& topic);
void createChart(String series, String page, String order, String templateName, String topic, size_t maxCount);
void clear();
void forEach(JsonHandler func);
}  // namespace Widgets
