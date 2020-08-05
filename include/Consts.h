#pragma once
/*
* Main consts
*/
#define FIRMWARE_VERSION "2.3.6-dev"
#define LED_PIN LED_BUILTIN
#define FLASH_4MB true
#define CHECK_UPDATE_INTERVAL_h 24

/*
* Optional modules
*/

/*
* RAM 48, ROM 16 348 
*/
#define OTA_UPDATES
/*
* RAM 48, ROM 16 348 
*/
// #define USE_MDNS

enum SchedulerTask { WIFI_SCAN,
                     NETWORK_CONNECTION,
                     SENSORS,
                     LOGGER,
                     TIME,
                     CHECK_UPDATE,
                     ANNOUNCE };

enum ErrorType_t {
    ET_NONE,
    ET_FUNCTION,
    ET_MODULE,
    ET_SYSTEM
};

enum ConfigType_t {
    CT_CONFIG,
    CT_SCENARIO
};

enum BusScanner_t {
    BS_I2C,
    BS_ONE_WIRE
};

enum BroadcastMessage_t {
    BM_ANNOUNCE,
    BM_MQTT_SETTINGS,
    NUM_BROADCAST_MESSAGES
};

enum LoopItems {
    LI_CLOCK,
    LT_FLAG_ACTION,
    LI_MQTT_CLIENT,
    LI_CMD,
    LI_ITEMS,
    LI_SCENARIO,
    LI_BROADCAST,
    LI_SERIAL,
    LT_TASKS,
    LT_LOGGER,
    NUM_LOOP_ITEMS
};

#define FILE_READ "r"
#define FILE_WRITE "w"
#define FILE_APPEND "a"
#define ONE_SECOND_ms 1000
#define ONE_SECOND_mu ONE_SECOND_ms * 1000
#define ONE_MINUTE_s 60
#define ONE_HOUR_m 60
#define ONE_HOUR_s ONE_HOUR_m* ONE_MINUTE_s
#define LEAP_YEAR(Y) (((1970 + Y) > 0) && !((1970 + Y) % 4) && (((1970 + Y) % 100) || !((1970 + Y) % 400)))
#define MIN_DATETIME 1575158400
#define ONE_MINUTE_ms ONE_MINUTE_s* ONE_SECOND_ms
#define ONE_HOUR_ms ONE_HOUR_m* ONE_MINUTE_ms

extern const char* getMessageType(BroadcastMessage_t message_type);