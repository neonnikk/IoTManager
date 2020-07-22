#pragma once

#include <Arduino.h>

#include "Config/ClockConfig.h"
#ifdef ESP8266
#include "sntp.h"
#endif

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

class Clock {
   public:
    Clock();
    void setConfig(ClockConfig* cfg);
    void loop();

    bool hasSynced() const;
    time_t getSystemTime() const;
    const unsigned long getEpoch();
    const String getTimeUnix();
    const Time_t* getLocal();
    /*
    * Локальное время "дд.ММ.гг"
    */
    const String getDateDotFormated();
    /*
    * Локальное дата время "дд.ММ.гг чч.мм.cc"
    */
    const String getDateTimeDotFormated();
    const String getDateTimeDotFormated(unsigned long);
    const String getDateTimeDotFormated(Time_t tm);
    /*
    * Локальное время "чч:мм:cc" , seconds = false то "чч:мм"
    */
    const String getTime(bool seconds = true);
    /*
    * Время с момента запуска "чч:мм:cc" далее "дд чч:мм"
    */
    const String getUptime();
    /*
    * Разбивает время на составляющие
    */
    void breakEpochToTime(unsigned long epoch, Time_t& tm);

   private:
    void startSync();
    void setupSntp();

   private:
    Time_t _time_local, _time_utc;
    unsigned long _uptime, _unixtime;
    bool _hasSynced, _configured;
    ClockConfig* _config;
    const uint8_t DAYS_IN_MONTH[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
};

extern Clock now;