#include "Cmd.h"

#include "Collection/Timers.h"

static const char *MODULE = TAG_TIMER;

void cmd_timerStart() {
    String name = getObjectName(TAG_TIMER, sCmd.next());
    String period = sCmd.next();

    Timers::add(name, parsePeriod(period));
}

void cmd_timerStop() {
    String name = getObjectName(TAG_TIMER, sCmd.next());

    Timers::erase(name);
}
