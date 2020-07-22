#pragma once

#include <SoftwareSerial.h>

#include "StringCommand.h"

#include "Consts.h"
#include "StringConsts.h"
#include "PrintMessage.h"
#include "Runtime.h"

#include "Objects/Terminal.h"
#include "Objects/Telnet.h"

#ifdef ESP8266
extern SoftwareSerial *mySerial;
#else
extern HardwareSerial *mySerial;
#endif
extern Terminal *term;
extern Telnet *telnet;

extern StringCommand sCmd;

bool parseRange(const String &str, Range &r);

bool parseMapParams(const String &str, MapParams &p);

Mapper* createMapper(const String& str);

unsigned long parsePeriod(const String &str, unsigned long default_multiplier = ONE_MINUTE_ms);

const String getObjectName(const String &objType, const String &objId);

void cmd_init();
/*
* Добавить комманду из строки в очередь
*/
void addCommand(const String &);
/*
* Добавить комманды из строки в очередь
*/
void addCommands(const String &);
/*
* Выполнить комманду
*/
void executeCommand(const String &);

void cmd_sensor();

void cmd_oneWire();

void cmd_pinSet();
void cmd_pinChange();

void cmd_switch();

void cmd_pwm();
void cmd_pwmSet();

void cmd_stepper();
void cmd_stepperSet();

void cmd_servo();
void cmd_servoSet();

void cmd_text();
void cmd_textSet();

void cmd_mqtt();
void cmd_http();
void cmd_push();

void cmd_button();
void cmd_buttonSet();
void cmd_buttonChange();

void cmd_log();

void cmd_timeSet();
void cmd_inputTime();
void cmd_inputDigit();
void cmd_digitSet();

void cmd_firmwareUpdate();
void cmd_firmwareVersion();
void cmd_reboot();

void cmd_levelPr();
void cmd_ultrasonicCm();

void cmd_bmp280P();
void cmd_bmp280T();

void cmd_bme280T();
void cmd_bme280P();
void cmd_bme280H();
void cmd_bme280A();

void cmd_dhtT();
void cmd_dhtH();
void cmd_dhtP();
void cmd_dhtComfort();
void cmd_dhtDewpoint();
void cmd_dhtPerception();

void cmd_serialBegin();
void cmd_serialEnd();
void cmd_serialWrite();
void cmd_serialLog();

void cmd_get();

void cmd_telnet();

void cmd_timerStop();
void cmd_timerStart();
