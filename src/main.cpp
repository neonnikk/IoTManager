#include "Global.h"

#include "Actions.h"
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
#include "Collection/Widgets.h"
#include "Runtime.h"
#include "MqttClient.h"
#include "HttpServer.h"
#include "WebClient.h"
#include "Objects/I2CScanner.h"
#include "Objects/OneWireScanner.h"
#include "TickerScheduler.h"
#include "Metric.h"
#include "Pins.h"

static const char* MODULE = "Main";

Metric m;
boolean initialized = false;

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

bool perform_logger_refresh_flag = false;
void perform_logger_refresh() {
    perform_logger_refresh_flag = true;
}

bool perform_logger_clear_flag = false;
void perform_logger_clear() {
    perform_logger_clear_flag = true;
}

void loop() {
    if (!initialized) {
        return;
    }
    metric.start();

    now.loop();

    loop_cmd();

    loop_items();

    if (config.general()->isScenarioEnabled()) {
        Scenario::loop();
    }

    if (config.general()->isBroadcastEnabled()) {
        Messages::loop();
        Broadcast::loop();
    }

    ts.update();

    Logger::update();

    if (config.hasChanged()) {
        Actions::execute(ACT_CONFIG_SAVE);
    }

    MqttClient::loop();

    ArduinoOTA.handle();

    // ws.cleanupClients();
    Actions::loop();

    Sensors::update();

    metric.finish();
}

void clock_task() {
    ts.add(
        TIME, ONE_SECOND_ms, [&](void*) {
            runtime.property(TAG_UPTIME, now.getUptime().c_str());

            if (now.hasSynced()) {
                runtime.property(TAG_TIME, now.getTime().c_str());
            }
        },
        nullptr, false);
}

void load_runtime() {
    runtime.load();
    runtime.property(TAG_DEVICEID, getDeviceId());
    runtime.property(TAG_FIRMWARE, FIRMWARE_VERSION);
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

void announce_task() {
    ts.add(
        ANNOUNCE, random(0.9 * config.general()->getBroadcastInterval() * ONE_SECOND_ms, config.general()->getBroadcastInterval()), [&](void*) {
            if (config.general()->isBroadcastEnabled()) {
                String data;
                data += getDeviceId();
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

    Pins::init();

    Actions::execute(ACT_CONFIG_LOAD, NULL, true);

    MqttClient::init();

    now.setConfig(config.clock());

    NetworkManager::init();

    load_runtime();

    cmd_init();

    clock_task();

    telemetry_task();

    announce_task();

    if (hasLastBootSucess()) {
        onStartCriticalBootSection();
        load_device_config();
    } else {
        replaceFileContent(DEVICE_COMMAND_FILE, "");
        replaceFileContent(DEVICE_SCENARIO_FILE, "");
    }
    onEndCriticalBootSection();

    initialized = true;
}

void load_device_config() {
    Widgets::clear();

    auto file = LittleFS.open(DEVICE_COMMAND_FILE, FILE_READ);
    if (!file) {
        pm.error("open " + String(DEVICE_COMMAND_FILE));
        return;
    }

    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.replace("\r", "");
        if (!line.startsWith("//") && !line.isEmpty()) {
            executeCommand(line);
        }
    }
    file.close();

    Scenario::init();
}

void load_device_preset(size_t num) {
    copyFile(getConfigFile(num, CT_CONFIG), DEVICE_COMMAND_FILE);
    copyFile(getConfigFile(num, CT_SCENARIO), DEVICE_SCENARIO_FILE);
    load_device_config();
}

void publish_widget_chart() {
    Logger::forEach([](LoggerTask* task) {
        task->readEntries([](LogMetadata* meta, uint8_t* data) {
            return MqttClient::publishChart(meta->getName(), LogEntry(data).asChartEntry());
        });
        return true;
    });
}

void add_to_config(const String& str) {
    addFile(DEVICE_COMMAND_FILE, str);
}
