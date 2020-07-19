#include "Cmd.h"

#include "Collection/Switches.h"

#include "PrintMessage.h"

static const char* MODULE = "switch";

void cmd_switch() {
    String name = sCmd.next();
    String assign = sCmd.next();
    String debounce = sCmd.next();

    auto* item = switches.add(name, assign);
    if (!item) {
        pm.error("bad switch");
        return;
    }
    item->setDebounce(debounce.toInt());

    String objName = "switch" + name;

    runtime.write(objName, item->getValue(), VT_INT);
}
