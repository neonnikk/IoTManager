#include "Cmd.h"

#include "StringConsts.h"
#include "PrintMessage.h"

#include "Collection/Buttons.h"
#include "Collection/Widgets.h"

static const char *MODULE = TAG_BUTTON;

static Button *getObjectByName(const String &name) {
    auto *obj = buttons.get(name);
    if (!obj) {
        pm.error("not found: " + name);
    }
    return obj;
}

static bool getButtonType(const String &assign, ButtonType_t &type) {
    if (isDigitStr(assign)) {
        type = BUTTON_GPIO;
    } else if (assign == "na") {
        type = BUTTON_VIRTUAL;
    } else if ((assign == "scen") || assign.startsWith("line")) {
        type = BUTTON_SCEN;
    } else {
        pm.error("bad assign: " + assign);
        return false;
    }
    return true;
}

void cmd_button() {
    String name = getObjectName(TAG_BUTTON, sCmd.next());

    String assign = sCmd.next();
    String descr = sCmd.next();
    String page = sCmd.next();
    String order = sCmd.next();
    String value = sCmd.next();
    String inverted = sCmd.next();

    value = value.isEmpty() ? "0" : value;
    inverted = inverted.isEmpty() ? "0" : inverted;

    ButtonType_t type;
    if (!getButtonType(assign, type)) {
        pm.error("wrong type");
        return;
    }

    buttons.add(type, name, assign, value, inverted);

    liveData.write(name, value, VT_INT);
    MqttClient::publishStatus(name, value, VT_INT);
    Widgets::createWidget(descr, page, order, "toggle", name);
}

void cmd_buttonChange() {
    String name = getObjectName(TAG_BUTTON, sCmd.next());
    auto *item = getObjectByName(name);
    if (!item) {
        return;
    }

    String value = item->toggleState();

    liveData.write(name, value, VT_INT);
    Scenario::fire(name);
    MqttClient::publishStatus(name, value, VT_INT);
}

void cmd_buttonSet() {
    String name = getObjectName(TAG_BUTTON, sCmd.next());
    auto *item = getObjectByName(name);
    if (!item) {
        return;
    }
    String value = sCmd.next();

    item->setValue(value);

    liveData.write(name, value, VT_INT);
    Scenario::fire(name);
    MqttClient::publishStatus(name, value, VT_INT);
}