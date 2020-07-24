#include "Cmd.h"

#include "Pins.h"
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

void cmd_button() {
    ParamStore params{sCmd.next()};
    String temlateOverride{sCmd.next()};

    String name = getObjectName(TAG_BUTTON, params.read(TAG_ID));
    String assign = params.read(TAG_PIN);
    bool inverted = params.readInt("inverted", false);
    bool state = params.readInt(TAG_STATE, LOW);

    auto item = buttons.add(name, assign);
    if (!item) {
        pm.error("on add: " + name);
        return;
    }

    item->setInverted(inverted);
    item->setValue(state);

    runtime.write(name, state);
    Widgets::createWidget(name, params, "toggle", temlateOverride);
}

void cmd_buttonChange() {
    String name = getObjectName(TAG_BUTTON, sCmd.next());

    auto item = getObjectByName(name);
    if (!item) {
        return;
    }

    runtime.write(name, item->toggleState());
}

void cmd_buttonSet() {
    String name = getObjectName(TAG_BUTTON, sCmd.next());
    int state = String(sCmd.next()).toInt();

    auto item = getObjectByName(name);
    if (!item) {
        return;
    }

    item->setValue(state);
    runtime.write(name, state);
}