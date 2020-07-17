#include "Collection/Widgets.h"

#include "Global.h"

#include "Base/KeyValueStore.h"

static const char* MODULE = "Widgets";

namespace Widgets {

std::vector<KeyValueFile*> _list;

const String getFilename(const String& name) {
    String res;
    res = "/widgets/";
    res += name;
    res += ".json";
    return res;
};

void createWidget(String& descr, String& page, const String& order, const String& templateName, const String& name) {
    KeyValueFile* widget = new KeyValueFile(getFilename(templateName).c_str());
    if (!widget->load()) {
        delete widget;
        return;
    }
    descr.replace("#", " ");
    widget->write("descr", descr);

    page.replace("#", " ");
    widget->write("page", page);
    
    widget->writeInt("order", order);

    String prefix = runtime.read("mqtt_prefix");
    widget->write("topic", prefix + "/" + name);

    _list.push_back(widget);
};

void createChart(String series, String page, String order, String templateName, String topic, size_t maxCount) {
    KeyValueFile* widget = new KeyValueFile(getFilename(templateName).c_str());
    if (!widget->load()) {
        delete widget;
        return;
    }

    widget->write("page", page);
    widget->writeInt("order", order);

    series.replace("#", " ");
    widget->write("series", series);
    widget->write("maxCount", maxCount);

    String prefix = runtime.read("mqtt_prefix");
    widget->write("topic", prefix + "/" + topic);

    _list.push_back(widget);
}

void clear() {
    _list.clear();
}

void forEach(JsonHandler func) {
    for (auto item : _list) {
        if (!func(item->asJson())) break;
    }
}
}  // namespace Widgets