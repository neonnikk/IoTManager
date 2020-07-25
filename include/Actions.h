#pragma once

#include <Arduino.h>

enum Act {
    ACT_CONFIG_LOAD,
    ACT_CONFIG_SAVE,
    ACT_SYSTEM_REBOOT,
    ACT_MQTT_RESTART,
    ACT_MQTT_BROADCAST,
    ACT_UPDATES_CHECK,
    ACT_UPGRADE,
    ACT_BUS_SCAN,
    ACT_LOGGER_REFRESH,
    ACT_LOGGER_CLEAR,
    ACT_WIDGET_PUBLISH,
    NUM_ACTIONS
};

namespace Actions {
void execute(Act act, void* param = NULL, bool immediately = false);
void loop();
}  // namespace Actions
