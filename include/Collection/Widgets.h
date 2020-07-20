#pragma once

#include <Arduino.h>
#include <functional>

typedef std::function<void(String)> JsonHandler;

namespace Widgets {
void createWidget(String& descr, String& page, const String& order, const String& templateName, const String& objName, const String templateOveride = "");
void createChart(String series, String page, String order, String templateName, String topic, int maxCount);
void clear();
void forEach(JsonHandler func);
}  // namespace Widgets
