#pragma once

//===========Firmware=============================================================================================================================================
#ifdef ESP8266
#define FIRMWARE_NAME "esp8266-iotm"
#define FIRMWARE_VERSION 267
#endif
#ifdef ESP32
#define FIRMWARE_NAME "esp32-iotm"
#define FIRMWARE_VERSION 267
#endif
#define FLASH_4MB true

//===========FSystem==============================================================================================================================================
#define NUM_BUTTONS 6
#define LED_PIN 2

//===========MQTT=================================================================================================================================================
#define MQTT_RECONNECT_INTERVAL 20000

//==========Telemetry=============================================================================================================================================
#define TELEMETRY_UPDATE_INTERVAL_MIN 60

//=========Configuration==========================================================================================================================================
#define DEVICE_CONFIG_FILE "s.conf.csv"
#define DEVICE_SCENARIO_FILE "s.scen.txt"


//=========System parts===========================================================================================================================================
//#define OTA_UPDATES_ENABLED
//#define MDNS_ENABLED
//#define WEBSOCKET_ENABLED
//#define LAYOUT_IN_RAM
//#define UDP_ENABLED
//#define SSDP_ENABLED

//=========Sensors enable/disable=================================================================================================================================
#define TANK_LEVEL_SAMPLES 10
#define LEVEL_ENABLED
#define ANALOG_ENABLED
#define DALLAS_ENABLED
#define DHT_ENABLED
#define BMP_ENABLED
#define BME_ENABLED


//=========Gears enable/disable===================================================================================================================================
#define STEPPER_ENABLED
#define SERVO_ENABLED


//========Other enable/disable====================================================================================================================================
#define LOGGING_ENABLED
#define SERIAL_ENABLED
#define PUSH_ENABLED

struct Time_t {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day_of_week;
    uint8_t day_of_month;
    uint8_t month;
    uint16_t day_of_year;
    uint16_t year;
    unsigned long days;
    unsigned long valid;
};

//================================================================================================================================================================
enum TimerTask_t { WIFI_SCAN,
                   WIFI_MQTT_CONNECTION_CHECK,
                   SENSORS10SEC,
                   SENSORS30SEC,
                   TIMER_COUNTDOWN,
                   TIME,
                   TIME_SYNC,
                   STATISTICS,
                   STATISTICS_WORK,
                   UPTIME,
                   UDP,
                   UDP_DB,
                   TEST };

enum NotAsyncActions {
    do_ZERO,
    do_UPGRADE,
    do_GETLASTVERSION,
    do_UDPDATAPARSE,
    do_MQTTUDP,
    do_BUSSCAN,
    do_MQTTPARAMSCHANGED,
    do_deviceInit,
    do_delChoosingItems,
    do_LAST,
};

enum LedStatus_t {
    LED_OFF,
    LED_ON,
    LED_SLOW,
    LED_FAST
};

enum ConfigType_t {
    CT_CONFIG,
    CT_SCENARIO
};

//07.11.2020
//RAM:   [=====     ]  46.8% (used 38376 bytes from 81920 bytes)     
//Flash: [=====     ]  54.2% (used 566004 bytes from 1044464 bytes)