#include "Global.h"

#include "Consts.h"
#include "StringConsts.h"
#include "Scenario.h"
#include "NetworkManager.h"
#include "Messages.h"
#include "Broadcast.h"
#include "Cmd.h"
#include "Collection/Sensors.h"
#include "Collection/Logger.h"
#include "Collection/Devices.h"
#include "Collection/Timers.h"
#include "MqttClient.h"
#include "HttpServer.h"
#include "WebClient.h"
#include "Sensors/I2CScanner.h"
#include "Sensors/OneWireScanner.h"
#include "TickerScheduler/Metric.h"

static const char* MODULE = "Main";

void flag_actions();
void config_backup();
void config_restore();

Metric m;
boolean initialized = false;
BusScanner* bus = NULL;

bool perform_mqtt_restart_flag = false;
void perform_mqtt_restart() {
    perform_mqtt_restart_flag = true;
}

bool perform_updates_check_flag = false;
void perform_updates_check() {
    perform_updates_check_flag = true;
}

bool perform_upgrade_flag = false;
void perform_upgrade() {
    perform_upgrade_flag = true;
}

bool broadcast_mqtt_settings_flag = false;
void broadcast_mqtt_settings() {
    broadcast_mqtt_settings_flag = true;
}

boolean perform_bus_scanning_flag = false;
BusScanner_t perform_bus_scanning_bus;
void perform_bus_scanning(BusScanner_t bus) {
    perform_bus_scanning_flag = true;
    perform_bus_scanning_bus = bus;
}

bool perform_system_restart_flag = false;
void perform_system_restart() {
    perform_system_restart_flag = true;
}

bool perform_logger_clear_flag = false;
void perform_logger_clear() {
    perform_logger_clear_flag = true;
}

void loop() {
    if (!initialized) {
        return;
    }
    m.loop();

    now.loop();
    m.add(LI_CLOCK);

    ArduinoOTA.handle();
    // ws.cleanupClients();

    flag_actions();
    m.add(LT_FLAG_ACTION);

    MqttClient::loop();
    m.add(LI_MQTT_CLIENT);

    loop_cmd();
    m.add(LI_CMD);

    loop_items();
    m.add(LI_ITEMS);

    if (config.general()->isScenarioEnabled()) {
        Scenario::loop();
        m.add(LI_SCENARIO);
    }

    if (config.general()->isBroadcastEnabled()) {
        Messages::loop();
        Broadcast::loop();
        m.add(LI_BROADCAST);
    }

    ts.update();
    m.add(LT_TASKS);

    Logger::update();
    m.add(LT_LOGGER);

    if (config.hasChanged()) {
        config_save();
    }
}

void flag_actions() {
    if (perform_mqtt_restart_flag) {
        MqttClient::reconnect();
        perform_mqtt_restart_flag = false;
    }

    if (perform_updates_check_flag) {
        runtime.write(TAG_LAST_VERSION, Updater::check());
        perform_updates_check_flag = false;
    }

    if (perform_upgrade_flag) {
        config_backup();
        bool res = Updater::upgrade_fs_image();
        if (res) {
            config_restore();
            if (Updater::upgrade_firmware()) {
                pm.info("done! restart...");
            }
        }
        perform_upgrade_flag = false;
    }

    if (broadcast_mqtt_settings_flag) {
        DynamicJsonBuffer jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        config.mqtt()->save(root);
        String buf;
        root.printTo(buf);
        Messages::post(BM_MQTT_SETTINGS, buf);
        broadcast_mqtt_settings_flag = false;
    }

    if (perform_bus_scanning_flag) {
        if (bus == NULL) {
            switch (perform_bus_scanning_bus) {
                case BS_I2C:
                    bus = new I2CScanner();
                    break;
                case BS_ONE_WIRE:
                    bus = new OneWireScanner();
                    break;
                default:
                    pm.error("unknown bus: " + String(perform_bus_scanning_bus, DEC));
            }
        }
        if (bus) {
            if (bus->scan()) {
                pm.info("scan done");
                runtime.write(bus->tag(), bus->results());
                perform_bus_scanning_flag = false;
                delete bus;
                bus = NULL;
            }
        }
    }

    if (perform_logger_clear_flag) {
        Logger::clear();
        perform_logger_clear_flag = false;
    }

    if (perform_system_restart_flag) {
        ESP.restart();
    }
}

void clock_task() {
    ts.add(
        TIME, ONE_SECOND_ms, [&](void*) {
            runtime.write(TAG_UPTIME, now.getUptime(), VT_STRING, KT_MQTT);
            if (now.hasSynced()) {
                runtime.write(TAG_TIME, now.getTime(), VT_STRING, KT_EVENT);
            }
        },
        nullptr, false);
}

String scenarioBackup, commandBackup, configBackup;
void config_backup() {
    readFile(DEVICE_SCENARIO_FILE, scenarioBackup);
    readFile(DEVICE_COMMAND_FILE, commandBackup);
    readFile(DEVICE_CONFIG_FILE, configBackup);
}

void config_restore() {
    writeFile(DEVICE_SCENARIO_FILE, scenarioBackup);
    writeFile(DEVICE_COMMAND_FILE, commandBackup);
    writeFile(DEVICE_CONFIG_FILE, configBackup);
    load_config();
}

void print_sys_timins() {
    m.print(Serial);
    m.reset();

    ts.print(Serial);
    ts.reset();
}

void load_runtime() {
    runtime.load();
    runtime.write("chipID", getChipId());
    runtime.write("firmware_version", FIRMWARE_VERSION);
    runtime.write("mqtt_prefix", config.mqtt()->getPrefix() + "/" + getChipId());
}

void telemetry_task() {
    if (!TELEMETRY_UPDATE_INTERVAL_s) {
        pm.info("Telemetry: disabled");
        return;
    }
    if (TELEMETRY_UPDATE_INTERVAL_s) {
        ts.add(
            STATISTICS, TELEMETRY_UPDATE_INTERVAL_s * ONE_SECOND_ms, [&](void*) {
                if (!NetworkManager::isNetworkActive()) {
                    return;
                }
                String url = "http://backup.privet.lv/visitors/?";
                WebClient::get(url);
            },
            nullptr, false);
    }
}

void sensors_task() {
    ts.add(
        SENSORS, ONE_SECOND_ms, [&](void*) {
            Sensors::update();
        },
        nullptr, false);
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

void onStartCriticalBootSection() {
    writeFile(DEVICE_BOOT_FILE, " ");
}

void onEndCriticalBootSection() {
    removeFile(DEVICE_BOOT_FILE);
}

bool hasLastBootSucess() {
    return !fileExists(DEVICE_BOOT_FILE);
}

void setup() {
    WiFi.setAutoConnect(false);
    WiFi.persistent(false);

    Serial.begin(115200);
    Serial.flush();
    Serial.println();
    Serial.println("--------------started----------------");

    fs_init();

    load_config();

    MqttClient::init();

    now.setConfig(config.clock());

    NetworkManager::init();

    load_runtime();

    cmd_init();

    clock_task();

    sensors_task();

    telemetry_task();

    announce_task();

    if (hasLastBootSucess()) {
        onStartCriticalBootSection();
        device_init();
    } else {
        replaceFileContent(DEVICE_COMMAND_FILE, "");
        replaceFileContent(DEVICE_SCENARIO_FILE, "");
    }
    onEndCriticalBootSection();

    initialized = true;
}