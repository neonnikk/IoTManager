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
#include "Collection/Widgets.h"
#include "Runtime.h"
#include "MqttClient.h"
#include "HttpServer.h"
#include "WebClient.h"
#include "Objects/I2CScanner.h"
#include "Objects/OneWireScanner.h"
#include "TickerScheduler.h"
#include "Metric.h"

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
        config_save();
    }

    flag_actions();

    MqttClient::loop();

    ArduinoOTA.handle();

    // ws.cleanupClients();

    metric.finish();
}

void flag_actions() {
    if (perform_logger_refresh_flag) {
        Logger::asCSV("/logs.csv");
        perform_logger_refresh_flag = false;
    }

    if (perform_mqtt_restart_flag) {
        MqttClient::reconnect();
        perform_mqtt_restart_flag = false;
    }

    if (perform_updates_check_flag) {
        runtime.property(TAG_LAST_VERSION, Updater::check());
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
            runtime.property(TAG_UPTIME, now.getUptime().c_str());

            if (now.hasSynced()) {
                runtime.property(TAG_TIME, now.getTime().c_str());
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
        load_device_config();
    } else {
        replaceFileContent(DEVICE_COMMAND_FILE, "");
        replaceFileContent(DEVICE_SCENARIO_FILE, "");
    }
    onEndCriticalBootSection();

    initialized = true;
}

void load_device_config() {
    auto file = LittleFS.open(DEVICE_COMMAND_FILE, FILE_READ);
    if (!file) {
        pm.error("open " + String(DEVICE_COMMAND_FILE));
        return;
    }
    Widgets::clear();
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.replace("\r", "");
        if (!line.startsWith("//") && !line.isEmpty()) {
            executeCommand(line);
        }
    }
    file.close();
}

void load_device_preset(size_t num) {
    copyFile(getConfigFile(num, CT_CONFIG), DEVICE_COMMAND_FILE);
    copyFile(getConfigFile(num, CT_SCENARIO), DEVICE_SCENARIO_FILE);
    load_device_config();
}

void config_save() {
    String buf;
    config.save(buf);
    writeFile(DEVICE_CONFIG_FILE, buf);
    config.setSynced();
}

void load_config() {
    String buf;
    if (readFile(DEVICE_CONFIG_FILE, buf)) {
        config.load(buf);
    }
}

void pubish_widget_collection() {
    auto file = LittleFS.open(DEVICE_LAYOUT_FILE, FILE_READ);
    if (file && file.available()) {
        while (file.available()) {
            String line = file.readStringUntil('\n');
            MqttClient::publistWidget(line);
        }
        file.close();
    }
}

void publish_widget_chart() {
    Logger::forEach([](LoggerTask* task) {
        task->readEntries([](LogMetadata* meta, uint8_t* data) {
            return MqttClient::publishChart(meta->getName(), LogEntry(data).asChartEntry());
        });
        return true;
    });
}

void config_add(const String& str) {
    addFile(DEVICE_COMMAND_FILE, str);
}
