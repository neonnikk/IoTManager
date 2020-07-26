#include "Actions.h"

#include <functional>

#include "Objects/I2CScanner.h"
#include "Objects/OneWireScanner.h"
#include "StringConsts.h"
#include "Runtime.h"
#include "Config.h"
#include "Collection/Logger.h"
#include "Collection/Widgets.h"
#include "Messages.h"
#include "MqttClient.h"
#include "Updater.h"
#include "PrintMessage.h"
#include "Utils/FileUtils.h"

static const char* MODULE = "Actions";

namespace Actions {

typedef std::function<bool(void*)> actionCallback;

struct ActionProc {
    actionCallback proc;
    void* param;
    bool runFlag;
    ActionProc(actionCallback cb) {
        proc = cb;
        param = NULL;
        runFlag = false;
    }
};

bool config_load(void* ptr = NULL);
bool config_save(void* ptr = NULL);
bool system_reboot(void* ptr = NULL);
bool mqtt_restart(void* ptr = NULL);
bool updates_check(void* ptr = NULL);
bool upgrade(void* ptr = NULL);
bool mqtt_broadcast_settings(void* ptr = NULL);
bool bus_scan(void* ptr);
bool logger_refresh(void* ptr = NULL);
bool logger_clear(void* ptr = NULL);
bool widgets_pubish(void* ptr = NULL);

ActionProc actions[NUM_ACTIONS] = {
    {config_load},
    {config_save},
    {system_reboot},
    {mqtt_restart},
    {mqtt_broadcast_settings},
    {updates_check},
    {upgrade},
    {bus_scan},
    {logger_refresh},
    {logger_clear},
    {widgets_pubish}};

void start(Act act) {
    bool actionComplete = actions[act].proc(actions[act].param);
    actions[act].runFlag = !actionComplete;
}

void execute(Act act, void* arg, bool immediately) {
    actions[act].param = arg;
    actions[act].runFlag = true;
    if (immediately) start(act);
}

void loop() {
    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        if (actions[i].runFlag) start(Act(i));
    }
}

bool config_load(void* ptr) {
    String buf;
    if (readFile(DEVICE_CONFIG_FILE, buf)) {
        config.load(buf);
    }
    return true;
}

bool config_save(void* ptr) {
    String buf;
    config.save(buf);
    writeFile(DEVICE_CONFIG_FILE, buf);
    config.setSynced();
    return true;
}

bool mqtt_restart(void* ptr) {
    MqttClient::reconnect();
    return true;
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
    config_load();
}

/*
* Виджеты
*/
bool widgets_pubish(void* ptr) {
    auto file = LittleFS.open(DEVICE_LAYOUT_FILE, FILE_READ);
    if (file.available()) {
        while (file.available()) {
            String line = file.readStringUntil('\n');
            Serial.print(line);
            MqttClient::publistWidget(line);
        }
    }
    file.close();
    return true;
}

/*
* Широковещательная расслыка настроек mqtt
*/
bool mqtt_broadcast_settings(void* ptr) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    config.mqtt()->save(root);
    String buf;
    root.printTo(buf);
    Messages::post(BM_MQTT_SETTINGS, buf);
    return true;
}

/**
 *  @brief  Сканирование шины
 *
 */
BusScanner* bus = NULL;
bool bus_scan(void* ptr) {
    bus = (BusScanner*)ptr;
    if (bus) {
        if (bus->scan()) {
            pm.info(bus->tag() + ":scan done");
            runtime.property(bus->tag(), bus->results());
            delete bus;
            bus = NULL;
            return true;
        } else {
            return false;
        }
    }
    return true;
}

/*
* Обновления файла с данными по логам
*/
bool logger_refresh(void* ptr) {
    Logger::asCSV("/logs.csv");
    return true;
}

/*
* Очистка журналов
*/
bool logger_clear(void* ptr) {
    Logger::clear();
    return true;
}

/*
* Произвести перезагрузку
*/
bool system_reboot(void* ptr) {
    ESP.restart();
    return true;
}

/*
* Проверка обновлений
*/
bool updates_check(void* ptr) {
    runtime.property(TAG_LAST_VERSION, Updater::check());
    return true;
}

/*
* Установка обновлений
*/
bool upgrade(void* ptr) {
    config_backup();
    bool res = Updater::upgrade_fs_image();
    if (res) {
        config_restore();
        if (Updater::upgrade_firmware()) {
            system_reboot();
        }
    }
    return true;
}

}  // namespace Actions