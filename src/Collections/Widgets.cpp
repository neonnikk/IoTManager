#include "Collection/Widgets.h"

#include "Config.h"
#include "Objects/Widget.h"
#include "Utils/SysUtils.h"

namespace Widgets {

std::vector<Widget*> _list;

const String getWidgetTopic(const String& objName) {
    String res = config.mqtt()->getPrefix();
    res += "/";
    res += getDeviceId();
    res += "/";
    res += objName;
    return res;
}

void createWidget(String& descr, String& page, const String& order, const String& templateName, const String& objName, const String templateOverride) {
    auto* widget = new Widget(templateName.c_str());
    if (!widget->load()) {
        delete widget;
        return;
    }
    if (!templateOverride.isEmpty()) {
        widget->fromJson(templateOverride);
    }

    descr.replace("#", " ");
    widget->write("descr", descr);

    page.replace("#", " ");
    widget->write("page", page);

    widget->writeAsInt("order", order);

    widget->write("topic", getWidgetTopic(objName));

    _list.push_back(widget);
};

void createChart(String series, String page, String order, String templateName, String objName, int maxCount) {
    auto* widget = new Widget(templateName.c_str());
    if (!widget->load()) {
        delete widget;
        return;
    }

    widget->write("page", page);
    widget->writeAsInt("order", order);

    series.replace("#", " ");
    widget->write("series", series);
    widget->write("maxCount", maxCount);

    widget->write("topic", getWidgetTopic(objName));

    _list.push_back(widget);
}

void clear() {
    for (auto item : _list) {
        delete item;
    }
    _list.clear();
}

void forEach(JsonHandler func) {
    for (auto item : _list) {
        func(item->asJson());
    }
}
}  // namespace Widgets