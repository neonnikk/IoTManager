#include "Collection/Widgets.h"

#include "Config.h"
#include "PrintMessage.h"
#include "StringConsts.h"
#include "Objects/Widget.h"
#include "Utils/SysUtils.h"
#include "Utils/FileUtils.h"

static const char* MODULE = "Widgets";

namespace Widgets {

const String getWidgetTopic(const String& objectName) {
    String res = config.mqtt()->getPrefix();
    res += "/";
    res += getDeviceId();
    res += "/";
    res += objectName;
    return res;
}

void createWidget(const String& objectName, const ParamStore& params, const char* defaultTemplate, const String& templateOverride) {
    String widgetTemplate = params.get("widget", defaultTemplate);
    auto widget = Widget(widgetTemplate);
    if (!widget.load()) {
        pm.error("on load: " + widgetTemplate);
        return;
    }

    widget.write("topic", getWidgetTopic(objectName));

    String descr = params.get("descr");
    descr.replace("#", " ");
    if (widgetTemplate.equals("chart")) {
        widget.write("series", descr);
        widget.write("maxCount", params.getInt("maxCount"));
    } else {
        widget.write("descr", descr);
    }

    String page = params.get("page");
    page.replace("#", " ");
    widget.write("page", page);

    int order = params.getInt("order");
    widget.write("order", order);

    if (!templateOverride.isEmpty()) {
        widget.fromJson(templateOverride.c_str());
    }
    auto file = LittleFS.open(DEVICE_LAYOUT_FILE, FILE_APPEND);
    if (file) {
        file.println(widget.asJson());
    }
    file.close();
}

void clear() {
    removeFile(DEVICE_LAYOUT_FILE);
}

}  // namespace Widgets