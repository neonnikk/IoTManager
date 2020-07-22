#include "Cmd.h"

#include "Global.h"

void cmd_reboot() {
    perform_system_restart();
}
