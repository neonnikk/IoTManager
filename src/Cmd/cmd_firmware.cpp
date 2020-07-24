#include "Cmd.h"

#include "Actions.h"

#include "Collection/Widgets.h"

void cmd_firmwareUpdate() {
    Actions::execute(ACT_UPGRADE);
}

void cmd_firmwareVersion() {
    String widget = sCmd.next();
    String page = sCmd.next();
    int order = String(sCmd.next()).toInt();

    //Widgets::createWidget(widget, page, order, "anydata", TAG_FIRMWARE);
}