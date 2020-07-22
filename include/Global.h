#pragma once

/*
* Libraries
*/
#include <Arduino.h>
#include <ArduinoJson.h>

#include "StringConsts.h"

#include "ESP32.h"
#include "ESP8266.h"
//

#include "Consts.h"
#include "Config.h"
#include "Updater.h"
#include "Clock.h"
#include "Scenario.h"
#include "MqttClient.h"
#include "NetworkManager.h"
#include "PrintMessage.h"

#include "Utils/FileUtils.h"
#include "Utils/JsonUtils.h"
#include "Utils/StringUtils.h"
#include "Utils/SysUtils.h"

#include <ArduinoOTA.h>
#include <time.h>


void perform_mqtt_restart();
/*
* Запрос на проверку обновлений
*/
extern void perform_updates_check();
/*
* Запрос на аггрейд
*/
extern void perform_upgrade();
/*
* Запрос обновления файла с данными по логам
*/
extern void perform_logger_refresh();
/*
* Запрос на очистук логов
*/
extern void perform_logger_clear();
/*
* Запрос на сканирование шины
*/
extern void perform_bus_scanning(BusScanner_t bus);
/*
* Произвести перезагрузку
*/
extern void perform_system_restart();
/*
* Широковещательная расслыка настроек mqtt
*/
extern void broadcast_mqtt_settings();
/*
* Запуск комманд из файла
*/
extern void fileExecute(const String filename);

extern void load_config();

extern void statistics_init();
extern void load_device_config();

extern void pubish_widget_collection();
extern void publish_widget_chart();

extern void config_add(const String &);
extern void config_save();

extern void load_device_preset(size_t num);

extern void loop_cmd();
extern void loop_items();

extern void init_mod();
extern void web_init();
extern void telemetry_task();
