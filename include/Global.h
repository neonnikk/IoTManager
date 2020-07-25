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

/*
* Запуск комманд из файла
*/
extern void fileExecute(const String filename);

extern void statistics_init();
extern void load_device_config();

extern void publish_widget_chart();

extern void add_to_config(const String &);

extern void load_device_preset(size_t num);

extern void loop_cmd();
extern void loop_items();

extern void init_mod();
extern void web_init();
extern void telemetry_task();
