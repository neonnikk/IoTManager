#include "Cmd.h"

#include "Actions.h"

#include "Collection/Widgets.h"

void cmd_firmwareUpdate() {
    Actions::execute(ACT_UPGRADE);
}

void cmd_firmwareVersion() {
    ParamStore params{sCmd.next()};
    String temlateOverride{sCmd.next()};

    Widgets::createWidget(TAG_FIRMWARE, params, "anydata", temlateOverride);
}