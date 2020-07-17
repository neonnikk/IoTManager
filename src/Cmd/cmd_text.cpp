#include "Cmd.h"

#include "Collection/Widgets.h"

void cmd_text() {
    String name = getObjectName(TAG_TEXT, sCmd.next());
    String value = sCmd.next();
    String page = sCmd.next();
    String order = sCmd.next();

    Widgets::createWidget(value, page, order, "anydata", name);

    liveData.write(name, value, VT_STRING);
    MqttClient::publishStatus(name, value, VT_STRING);
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

    liveData.write(name, value, VT_STRING);
    MqttClient::publishStatus(name, value, VT_STRING);
}