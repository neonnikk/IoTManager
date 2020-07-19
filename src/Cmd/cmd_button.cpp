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
    if (assign.isEmpty()) {
        type = BUTTON_VIRTUAL;
    } else if (isDigitStr(assign)) {
        type = BUTTON_GPIO;
    } else if ((assign == "scen") || assign.startsWith("line")) {
        type = BUTTON_SCEN;
    } else {
        pm.error("bad assign: " + assign);
        return false;
    }
    return true;
}

void cmd_button() {
    KeyValueStore *params = new KeyValueStore(sCmd.next());

    String name = getObjectName(TAG_BUTTON, params->read("id").c_str());
    String descr = params->read("name");
    String assign = params->read("pin");
    String inverted = params->read("inverted", "false");
    String state = params->read("state", "0");
    String widget = params->read("widget", "toggle");
    String page = params->read("page");
    String order = params->read("order");

    delete params;

    ButtonType_t type;
    if (!getButtonType(assign, type)) {
        pm.error("wrong type");
        return;
    }

    buttons.add(type, name, assign, state, inverted);

    runtime.write(name, state, VT_INT);

    Widgets::createWidget(descr, page, order, widget, name);
}

void cmd_buttonChange() {
    String name = getObjectName(TAG_BUTTON, sCmd.next());
    auto *item = getObjectByName(name);
    if (!item) {
        return;
    }
    String state = item->toggleState();

    runtime.write(name, state, VT_INT);
}

void cmd_buttonSet() {
    String name = getObjectName(TAG_BUTTON, sCmd.next());
    String state = sCmd.next();
    auto *item = getObjectByName(name);
    if (!item) {
        return;
    }

    item->setValue(state);

    runtime.write(name, state, VT_INT);
}