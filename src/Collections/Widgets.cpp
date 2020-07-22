#include "Collection/Widgets.h"

#include "Config.h"
#include "PrintMessage.h"
#include "Objects/Widget.h"
#include "Utils/SysUtils.h"

static const char* MODULE = "Widgets";

namespace Widgets {

std::vector<Widget*> _list;

const String getWidgetTopic(const String& objectName) {
    String res = config.mqtt()->getPrefix();
    res += "/";
    res += getDeviceId();
    res += "/";
    res += objectName;
    return res;
}

void createWidget(const String& objectName, const ParamStore& params, const char* defaultTemplate, const String& templateOverride) {
    String widgetTemplate = params.read("widget", defaultTemplate);
    auto* widget = new Widget(widgetTemplate);
    if (!widget->load()) {
        pm.error("on load: " + widgetTemplate);
        delete widget;
        return;
    }

    widget->write("topic", getWidgetTopic(objectName));

    String descr = params.read("descr");
    descr.replace("#", " ");
    if (widgetTemplate.equals("chart")) {
        widget->write("series", descr);
        widget->write("maxCount", params.readInt("maxCount"));
    } else {
        widget->write("descr", descr);
    }

    String page = params.read("page");
    page.replace("#", " ");
    widget->write("page", page);

    int order = params.readInt("order");
    widget->write("order", order);

    if (!templateOverride.isEmpty()) {
        widget->fromJson(templateOverride);
    }

    _list.push_back(widget);
}

void clear() {
    for (auto item : _list) {
        delete item;
    }
    _list.clear();
}

void forEach(WidgetHandler func) {
    for (auto item : _list) {
        func(item);
    }
}
}  // namespace Widgets