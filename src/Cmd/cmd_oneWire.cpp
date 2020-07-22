#include "Cmd.h"

#include "Objects/OneWireBus.h"

void cmd_oneWire() {
    int pin = String(sCmd.next()).toInt();

    onewire.attach(pin);
}
