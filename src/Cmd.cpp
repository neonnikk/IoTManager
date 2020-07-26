#include "Cmd.h"

#include "Base/StringQueue.h"

#include "Collection/Buttons.h"
#include "Collection/Devices.h"
#include "Collection/Logger.h"
#include "Collection/Sensors.h"
#include "Collection/Servos.h"
#include "Collection/Switches.h"
#include "Collection/Timers.h"
#include "Collection/Widgets.h"

#include "Objects/OneWireBus.h"
#include "Objects/Pwm.h"
#include "Objects/Terminal.h"
#include "Objects/Telnet.h"

#include "StringConsts.h"
#include "MqttClient.h"
#include "WebClient.h"
#include "Scenario.h"

static const char *MODULE = "Cmd";

std::list<String> _commands;

StringCommand sCmd;

#ifdef ESP8266
SoftwareSerial *mySerial = NULL;
#else
HardwareSerial *mySerial = NULL;
#endif

Terminal *term = NULL;
Telnet *telnet = NULL;

unsigned long parsePeriod(const String &str, unsigned long default_time_syfix) {
    unsigned long res = 0;
    if (str.indexOf("digit") != -1) {
        res = runtime.getInt(str);
    } else {
        if (str.endsWith("ms")) {
            res = str.substring(0, str.indexOf("ms")).toInt();
        } else if (str.endsWith("s")) {
            res = str.substring(0, str.indexOf("s")).toInt() * ONE_SECOND_ms;
        } else if (str.endsWith("m")) {
            res = str.substring(0, str.indexOf("m")).toInt() * ONE_MINUTE_ms;
        } else if (str.endsWith("h")) {
            res = str.substring(0, str.indexOf("h")).toInt() * ONE_HOUR_ms;
        } else {
            res = str.toInt() * default_time_syfix;
        }
    }
    return res;
}

const String getObjectName(const String &objType, const String &objId) {
    return objType + objId;
}

void cmd_init() {
    switches.setOnChangeState([](Switch *obj) {
        String name = String("switch") + obj->getName();
        runtime.writeAsInt(name, obj->getValue());
    });

    sCmd.setDefaultHandler([](const char *str) {
        pm.error("unknown: " + String(str));
    });

    sCmd.addCommand("button", cmd_button);
    sCmd.addCommand("buttonSet", cmd_buttonSet);
    sCmd.addCommand("buttonChange", cmd_buttonChange);

    sCmd.addCommand("switch", cmd_switch);

    sCmd.addCommand("pinSet", cmd_pinSet);
    sCmd.addCommand("pinChange", cmd_pinChange);

    sCmd.addCommand("pwm", cmd_pwm);
    sCmd.addCommand("pwmSet", cmd_pwmSet);

    sCmd.addCommand("sensor", cmd_sensor);

    sCmd.addCommand("levelPr", cmd_levelPr);

    sCmd.addCommand("dhtT", cmd_dhtT);
    sCmd.addCommand("dhtH", cmd_dhtH);
    sCmd.addCommand("dhtPerception", cmd_dhtPerception);
    sCmd.addCommand("dhtComfort", cmd_dhtComfort);
    sCmd.addCommand("dhtDewpoint", cmd_dhtDewpoint);

    sCmd.addCommand("bmp280T", cmd_bmp280T);
    sCmd.addCommand("bmp280P", cmd_bmp280P);

    sCmd.addCommand("bme280T", cmd_bme280T);
    sCmd.addCommand("bme280P", cmd_bme280P);
    sCmd.addCommand("bme280H", cmd_bme280H);
    sCmd.addCommand("bme280A", cmd_bme280A);

    sCmd.addCommand("stepper", cmd_stepper);
    sCmd.addCommand("stepperSet", cmd_stepperSet);

    sCmd.addCommand("servo", cmd_servo);
    sCmd.addCommand("servoSet", cmd_servoSet);

    sCmd.addCommand("serialBegin", cmd_serialBegin);
    sCmd.addCommand("serialEnd", cmd_serialEnd);
    sCmd.addCommand("serialWrite", cmd_serialWrite);

    sCmd.addCommand("serialLog", cmd_serialLog);

    sCmd.addCommand("telnet", cmd_telnet);

    sCmd.addCommand("log", cmd_log);

    sCmd.addCommand("inputDigit", cmd_inputDigit);
    sCmd.addCommand("digitSet", cmd_digitSet);

    sCmd.addCommand("inputTime", cmd_inputTime);
    sCmd.addCommand("timeSet", cmd_timeSet);

    sCmd.addCommand("timerStart", cmd_timerStart);
    sCmd.addCommand("timerStop", cmd_timerStop);

    sCmd.addCommand("text", cmd_text);
    sCmd.addCommand("textSet", cmd_textSet);

    sCmd.addCommand("mqtt", cmd_mqtt);
    sCmd.addCommand("http", cmd_http);

    sCmd.addCommand("push", cmd_push);

    sCmd.addCommand("firmwareUpdate", cmd_firmwareUpdate);
    sCmd.addCommand("firmwareVersion", cmd_firmwareVersion);

    sCmd.addCommand("get", cmd_get);

    sCmd.addCommand("reboot", cmd_reboot);

    sCmd.addCommand("oneWire", cmd_oneWire);
}

//levelPr p 14 12 Вода#в#баке,#% Датчики fillgauge 125 20 1
void cmd_levelPr() {
    String name = sCmd.next();
    String trig = sCmd.next();
    String echo = sCmd.next();
    String widget_name = sCmd.next();
    String page_name = sCmd.next();
    String type = sCmd.next();
    String empty_level = sCmd.next();
    String full_level = sCmd.next();

    // Ultrasonic::levelPr_value_name = name;

    // options.write("e_lev", empty_level);
    // options.write("f_lev", full_level);
    // options.write("trig", trig);
    // options.write("echo", echo);
    // pinMode(trig.toInt(), OUTPUT);
    // pinMode(echo.toInt(), INPUT);
    // Widgets::createWidget(widget_name, page_name, order, type, name);
}

//dhtH h 2 dht11 Влажность#DHT,#t°C Датчики any-data 1
void cmd_dhtH() {
    String name = sCmd.next();
    String pin = sCmd.next();
    // DHTSensor::dhtH_value_name = name;
    // if (sensor_type == "dht11") {
    //     DHTSensor::dht.setup(pin.toInt(), DHTesp::DHT11);
    // }
    // if (sensor_type == "dht22") {
    //     DHTSensor::dht.setup(pin.toInt(), DHTesp::DHT22);
    // }
    // Widgets::createWidget(widget_name, page_name, order, type, name);
}

// dhtT t 2 dht11 Температура#DHT,#t°C Датчики any-data 1
void cmd_dhtT() {
    String name = sCmd.next();
    String pin = sCmd.next();
    // DHTSensor::dhtT_value_name = name;
    // if (sensor_type == "dht11") {
    //     DHTSensor::dht.setup(pin.toInt(), DHTesp::DHT11);
    // }
    // if (sensor_type == "dht22") {
    //     DHTSensor::dht.setup(pin.toInt(), DHTesp::DHT22);
    // }

    // Widgets::createWidget(widget_name, page_name, order, type, name);
}

//dhtDewpoint Точка#росы: Датчики 5
void cmd_dhtDewpoint() {
    String widget_name = sCmd.next();
    String page_name = sCmd.next();

    // Widgets::createWidget(widget_name, page_name, order, "anydata", "dhtDewpoint");
}

// dhtPerception Восприятие: Датчики 4
void cmd_dhtPerception() {
    // Widgets::createWidget(widget_name, page_name, order, "any-data", "dhtPerception");
}

// dhtComfort Степень#комфорта: Датчики 3
void cmd_dhtComfort() {
    // Widgets::createWidget(widget_name, page_name, order, "anydata", "dhtComfort");
}

// bmp280T temp1 0x76 Температура#bmp280 Датчики any-data 1
void cmd_bmp280T() {
    // BMP280Sensor::bmp280T_value_name = name;
    // BMP280Sensor::bmp.begin(hexStringToUint8(address));
    // BMP280Sensor::bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
    //                               Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
    //                               Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
    //                               Adafruit_BMP280::FILTER_X16,      /* Filtering. */
    //                               Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

    // Widgets::createWidget(widget_name, page_name, order, type, name);
}

//bmp280P press1 0x76 Давление#bmp280 Датчики any-data 2
void cmd_bmp280P() {
    // BMP280Sensor::bmp280P_value_name = name;
    // BMP280Sensor::bmp.begin(hexStringToUint8(address));
    // BMP280Sensor::bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
    //                               Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
    //                               Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
    //                               Adafruit_BMP280::FILTER_X16,      /* Filtering. */
    //                               Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

    // Widgets::createWidget(widget_name, page_name, order, type, name);
}

//bme280T temp1 0x76 Температура#bmp280 Датчики any-data 1
void cmd_bme280T() {
    // Widgets::createWidget(descr, page, order, type, name);
}

//bme280P pres1 0x76 Давление#bmp280 Датчики any-data 1
void cmd_bme280P() {
    // Widgets::createWidget(widget_name, page_name, order, type, name);
}

//bme280H hum1 0x76 Влажность#bmp280 Датчики any-data 1
void cmd_bme280H() {
}

//bme280A altit1 0x76 Высота#bmp280 Датчики any-data 1
void cmd_bme280A() {
}

bool extractCommand(const String &buf, size_t &startIndex, String &block) {
    int endIndex = buf.indexOf("\n", startIndex);
    if (endIndex < 0) {
        return false;
    }
    block = buf.substring(startIndex, endIndex);
    block.replace("\r\n", "");
    block.replace("\r", "");
    if (block.startsWith("//")) {
        block = "";
    }
    startIndex = endIndex + 1;
    return true;
}

// 1-100
bool parseRange(const String &str, Range &r) {
    int split = str.indexOf("-");
    if (split < 0) {
        pm.error("wrong range");
        return false;
    }
    r.min = str.substring(0, split).toInt();
    r.max = str.substring(split + 1).toInt();
    return true;
}

// 1-1023/1-100
bool parseMapParams(const String &str, MapParams &p) {
    int split = str.indexOf("/");
    if (split < 0) {
        pm.error("wrong map");
        return false;
    }
    return parseRange(str.substring(0, split), p.in) && parseRange(str.substring(split + 1), p.out);
}

Mapper *createMapper(const String &str) {
    if (!str.isEmpty()) {
        MapParams p;
        if (parseMapParams(str, p)) {
            return new Mapper{p};
        }
    }
    return NULL;
}

void addCommands(const String &str) {
    size_t pos = 0;
    while (pos < str.length()) {
        String buf;
        if (!extractCommand(str, pos, buf)) {
            break;
        }
        if (!buf.isEmpty()) {
            addCommand(buf);
        }
    }
}

void addCommand(const String &str) {
    _commands.push_back(str);
}

void executeCommand(const String &str) {
    pm.info("Execute: " + str);
    sCmd.readStr(str);
}

void loop_cmd() {
    if (!_commands.empty()) {
        String cmd = _commands.front();
        executeCommand(cmd);
        _commands.pop_front();
    }
}

void loop_items() {
    if (term) {
        term->loop();
    }
    if (telnet) {
        telnet->loop();
    }

    switches.loop();
}
