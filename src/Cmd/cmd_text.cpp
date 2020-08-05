#include "Cmd.h"

#include "Clock.h"
#include "Collection/Widgets.h"

void cmd_text() {
    ParamStore params{sCmd.next()};
    String temlateOverride{sCmd.next()};

    String objName = getObjectName(TAG_TEXT, params.get(TAG_ID));
    String value = params.get("value");
    runtime.write(objName, value);
    Widgets::createWidget(objName, params, "anydata", temlateOverride);
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