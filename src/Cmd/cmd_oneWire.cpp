#include "Cmd.h"

#include "Objects/OneWireBus.h"

static const char* MODULE = TAG_ONE_WIRE;

void cmd_oneWire() {
    String assign = String(sCmd.next());
    if (!Pins::isValid(assign)) {
        pm.error("invalid assign '" + assign + "'");
        return;
    }
    uint8_t pin = assign.toInt();
    if (Pins::isBusy(pin)) {
        pm.error("is busy");
        return;
    }
    oneWire.attach(pin);
    Actions::execute(ACT_BUS_SCAN, new OneWireScanner());
}
