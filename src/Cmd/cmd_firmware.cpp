#include "Cmd.h"

#include "Global.h"

#include "Collection/Widgets.h"

void cmd_firmwareUpdate() {
    perform_upgrade();
}

void cmd_firmwareVersion() {
    String widget = sCmd.next();
    String page = sCmd.next();
    int order = String(sCmd.next()).toInt();

    //Widgets::createWidget(widget, page, order, "anydata", TAG_FIRMWARE);
}