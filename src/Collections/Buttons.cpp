#include "Collection/Buttons.h"

#include "Pins.h"
#include "StringConsts.h"
#include "PrintMessage.h"

static const char* MODULE = TAG_BUTTON;

Collection<Button> buttons;

template <>
Button* Collection<Button>::add(const String& name, const String& assign) {
    Button* item;
    if (assign.isEmpty()) {
        item = new VirtualButton(name, assign);
    } else if (Pins::isValid(assign)) {
        item = new PinButton(name, assign);
    } else {
        pm.error("bad assign: " + assign);
    }
    if (item) {
        _list.push_back(item);
        return last();
    }
    return NULL;
}
