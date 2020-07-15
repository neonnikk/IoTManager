#include "Global.h"

#include "Messages.h"
#include "Broadcast.h"
#include "WebClient.h"
#include "Scenario.h"
#include "MqttClient.h"
#include "NetworkManager.h"

#include "Collection/Logger.h"
#include "Collection/Sensors.h"
#include "Collection/Timers.h"

#include "Utils/TimeUtils.h"

static const char* MODULE = "Init";

void sensors_task() {
    ts.add(
        SENSORS, 1000, [&](void*) {
            Sensors::update();
        },
        nullptr, true);
}

void announce_task() {
    ts.add(
        ANNOUNCE, random(0.9 * config.general()->getBroadcastInterval() * ONE_SECOND_ms, config.general()->getBroadcastInterval()), [&](void*) {
            if (config.general()->isBroadcastEnabled()) {
                String data;
                data += getChipId();
                data += ";";
                data += config.general()->getBroadcastName();
                data += ";";
                data += NetworkManager::getHostIP().toString();
                data += ";";

                Messages::post(BM_ANNOUNCE, data);
            }
        },
        nullptr, false);
}

void timers_task() {
    ts.add(
        TIMERS, ONE_SECOND_ms, [&](void*) { Timers::update(); }, nullptr, true);
}

void uptime_task() {
    ts.add(
        UPTIME, 5 * ONE_SECOND_ms, [&](void*) { runtime.write("uptime", now.getUptime()); }, nullptr, true);
}

void init_mod() {
    MqttClient::init();

    Logger::init();

    cmd_init();

    sensors_task();

    timers_task();

    uptime_task();

    announce_task();

    ts.add(
        SYS_MEMORY, 10 * ONE_SECOND_ms, [&](void*) { print_sys_memory(); }, nullptr, false);

    ts.add(
        SYS_TIMINGS, ONE_MINUTE_ms, [&](void*) { print_sys_timins(); }, nullptr, false);

    if (NetworkManager::isNetworkActive()) {
        perform_updates_check();
    };

    if (!fileExists(DEVICE_BOOT_FILE)) {
        writeFile(DEVICE_BOOT_FILE, " ");
        device_init();
    } else {
        String name = String(DEVICE_COMMAND_FILE) + ".bak";
        copyFile(DEVICE_COMMAND_FILE, name);
        writeFile(DEVICE_COMMAND_FILE, name);

        name = String(DEVICE_SCENARIO_FILE) + ".bak";
        copyFile(DEVICE_SCENARIO_FILE, name);
        writeFile(DEVICE_SCENARIO_FILE, name);
    }
    removeFile(DEVICE_BOOT_FILE);
}

void telemetry_init() {
    if (!TELEMETRY_UPDATE_INTERVAL) {
        pm.info("Telemetry: disabled");
        return;
    }
    if (TELEMETRY_UPDATE_INTERVAL) {
        ts.add(
            STATISTICS, TELEMETRY_UPDATE_INTERVAL, [&](void*) {
                if (!NetworkManager::isNetworkActive()) {
                    return;
                }
                String url = "http://backup.privet.lv/visitors/?";
                WebClient::get(url);
            },
            nullptr, true);
    }
}