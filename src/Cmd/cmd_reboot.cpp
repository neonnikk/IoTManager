#include "Cmd.h"

#include "Actions.h"

void cmd_reboot() {
    Actions::execute(ACT_SYSTEM_REBOOT);
}
