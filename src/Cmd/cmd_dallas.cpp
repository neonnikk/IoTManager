#include "Cmd.h"

#include "Objects/OneWireBus.h"

#include "Collection/Sensors.h"
#include "Collection/Widgets.h"

static const char *MODULE = "dallas";

// dallas temp1 0x14 Температура Датчики anydata 1
// dallas temp2 0x15 Температура Датчики anydata 2
void cmd_dallas() {
    if (!onewire.attached()) {
        pm.error("attach bus first");
        return;
    }

    String name = sCmd.next();
    String address = sCmd.next();
    String descr = sCmd.next();
    String page = sCmd.next();
    String widget = sCmd.next();
    String order = sCmd.next();

    Sensors::add(SensorType_t::DALLAS, name, address);

    Widgets::createWidget(descr, page, order, widget, name);
}
