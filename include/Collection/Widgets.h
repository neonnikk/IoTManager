#pragma once

#include <Arduino.h>
#include <functional>

#include "Objects/Widget.h"
#include "Base/ParamStore.h"

typedef std::function<void(Widget*)> WidgetHandler;

namespace Widgets {
void createWidget(const String& objectName, const ParamStore& params, const char* defaultTemplate, const String& templateOverride);
void clear();
void forEach(WidgetHandler func);
}  // namespace Widgets
