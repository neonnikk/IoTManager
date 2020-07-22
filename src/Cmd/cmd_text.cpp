#include "Cmd.h"

#include "Clock.h"
#include "Collection/Widgets.h"

void cmd_text() {
    String name = getObjectName(TAG_TEXT, sCmd.next());
    String value = sCmd.next();
    String page = sCmd.next();
    int order = String(sCmd.next()).toInt();

    runtime.write(name, value);
    // Widgets::createWidget(value, page, order, "anydata", name);
}

void cmd_textSet() {
    String name = getObjectName(TAG_TEXT, sCmd.next());
    String value = sCmd.next();

    value.replace("_", " ");
    value.replace("#", " ");

    if (value.indexOf("-time") >= 0) {
        value.replace("-time", "");
        value = value + " " + now.getDateTimeDotFormated();
    }

    runtime.write(name, value);
}