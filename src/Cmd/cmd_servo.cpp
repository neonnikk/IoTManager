#include "Cmd.h"

#include "Collection/Servos.h"

#include "PrintMessage.h"

static const char* MODULE = "servos";

void cmd_servoSet() {
    String name = sCmd.next();
    String value = sCmd.next();

    String objName = "servo" + name;

    auto* item = servos.get(objName);
    if (!item) {
        return;
    }
    item->setValue(value);

    runtime.writeAsInt(objName, value);
}