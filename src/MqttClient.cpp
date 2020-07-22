#include "MqttClient.h"

#include <ESP.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <StreamString.h>

#include "Global.h"
#include "Cmd.h"
#include "Config.h"
#include "NetworkManager.h"
#include "Runtime.h"

#include "Collection/Logger.h"

#include "Utils/TimeUtils.h"
#include "Utils/SysUtils.h"

static const char* MODULE = "Mqtt";

namespace MqttClient {

WiFiClient espClient;
PubSubClient _mqtt(espClient);

struct MqttMessage {
   private:
    char* _topic;
    char* _data;

   public:
    MqttMessage(const MqttMessage& b) : _topic{NULL}, _data{NULL} {
        _topic = strdup(b._topic);
        _data = strdup(b._data);
    }

    MqttMessage(const char* topic, const char* data) : _topic{NULL}, _data{NULL} {
        _topic = strdup(topic);
        _data = strdup(data);
    }

    ~MqttMessage() {
        if (_topic) free(_topic);
        if (_data) free(_data);
    }

    bool isValid() const {
        return _topic && _data;
    }

    const char* getTopic() const {
        return _topic;
    }

    const char* getData() const {
        return _data;
    }
};

std::list<MqttMessage> _queue;

static unsigned long RECONNECT_INTERVAL = 5000;
static size_t RECONNECT_ATTEMPTS_MAX = 10;
unsigned long _lastConnetionAttempt = 0;
size_t _connectionAttempts = 0;

String _deviceRoot;
String _addr;
int _port;
String _user;
String _pass;
String _uuid;
String _prefix;

MqttWriter* getWriter(const char* topic) {
    String path = _deviceRoot;
    path += topic;
    return new MqttWriter(&_mqtt, path.c_str());
}

void handleSubscribedUpdates(char* topic, uint8_t* payload, size_t length);

void init() {
    _addr = config.mqtt()->getServer();
    _port = config.mqtt()->getPort();
    _user = config.mqtt()->getUser();
    _pass = config.mqtt()->getPass();
    _prefix = config.mqtt()->getPrefix();

    _uuid = getDeviceId();
    _deviceRoot = _prefix;
    _deviceRoot += "/";
    _deviceRoot += _uuid;
    _deviceRoot += "/";
}

bool connect() {
    bool res = false;
    _mqtt.setServer(_addr.c_str(), _port);
    res = _mqtt.connect(_uuid.c_str(), _user.c_str(), _pass.c_str());
    if (res) {
        pm.info("connected " + _addr + ":" + String(_port, DEC) + " " + _prefix);
        _mqtt.setCallback(handleSubscribedUpdates);
        _mqtt.subscribe(_prefix.c_str());
        _mqtt.subscribe((_deviceRoot + "+/" + "control").c_str());
        _mqtt.subscribe((_deviceRoot + "order").c_str());
        _mqtt.subscribe((_deviceRoot + "update").c_str());
        _mqtt.subscribe((_deviceRoot + "devc").c_str());
        _mqtt.subscribe((_deviceRoot + "devs").c_str());
    } else {
        pm.error("could't connect: " + getStateStr());
    }
    return res;
}

void disconnect() {
    pm.info("disconnected");
    _mqtt.disconnect();
}

void reconnect() {
    disconnect();
    init();
    connect();
}

boolean _mqtt_publish(const char* topic, const char* data) {
    if (!_mqtt.connected()) {
        pm.error("not connected");
        return false;
    }
    if (_mqtt.beginPublish(topic, strlen(data), false)) {
        _mqtt.print(data);
        return _mqtt.endPublish();
    }
    pm.error("on publish");
    return false;
}

void pushToQueue(const String& topic, const String& data) {
    _queue.push_back(MqttMessage{topic.c_str(), data.c_str()});
}

bool isConnected() {
    return _mqtt.connected();
}

bool hasAttempts() {
    return !RECONNECT_ATTEMPTS_MAX || (_connectionAttempts < RECONNECT_ATTEMPTS_MAX);
}
bool publishStarted = false;
void loop() {
    if (!NetworkManager::isNetworkActive()) {
        return;
    }

    _mqtt.loop();

    if (!config.mqtt()->isEnabled()) {
        if (isConnected()) {
            disconnect();
        }
        return;
    }
    if (!isConnected()) {
        if (!hasAttempts()) {
            return;
        }
        if (millis_since(_lastConnetionAttempt) < RECONNECT_INTERVAL) {
            return;
        }
        init();
        connect();
        _lastConnetionAttempt = millis();
    }

    if (!isConnected()) {
        pm.error(getStateStr());
        _connectionAttempts++;
        if (!hasAttempts()) {
            config.mqtt()->enable(false);
        }
        return;
    }

    _connectionAttempts = 0;

    if (!_queue.empty()) {
        auto message = _queue.front();
        if (message.isValid()) {
            _mqtt.publish(message.getTopic(), message.getData());
        }
        _queue.pop_front();
    }
}

const String parseControl(const String& str) {
    String res;
    size_t len = str.length();
    String num1 = str.substring(len - 1);
    String num2 = str.substring(len - 2, len - 1);
    if (isDigitStr(num1) && isDigitStr(num2)) {
        res = str.substring(0, len - 2) + "Set" + num2 + num1;
    } else {
        if (isDigitStr(num1)) {
            res = str.substring(0, len - 1) + "Set" + num1;
        }
    }
    return res;
}

void handleSubscribedUpdates(char* topic, uint8_t* payload, size_t length) {
    pm.info("<= " + String(topic));

    String topicStr = String(topic);
    StreamString payloadStr;
    payloadStr.write(payload, length);

    if (payloadStr.equalsIgnoreCase("hello")) {
        runtime.publish();
        pubish_widget_collection();
        publish_widget_chart();
    } else if (topicStr.indexOf("control")) {
        // название топика -  команда,
        // значение - параметр
        //IoTmanager/800324-1458415/button99/control 1
        String topic = selectFromMarkerToMarker(topicStr, "/", 3);
        topic = parseControl(topic);
        String number = selectToMarkerLast(topic, "Set");
        topic.replace(number, "");
        addCommand(topic + " " + number + " " + payloadStr);
    } else if (topicStr.indexOf("order")) {
        payloadStr.replace("_", " ");
        addCommand(payloadStr);
    } else if (topicStr.indexOf("update")) {
        if (payloadStr == "1") {
            perform_upgrade();
        }
    } else if (topicStr.indexOf("devc")) {
        writeFile(DEVICE_COMMAND_FILE, payloadStr);
        load_device_config();
    } else if (topicStr.indexOf("devs")) {
        writeFile(DEVICE_SCENARIO_FILE, payloadStr);
        Scenario::reinit();
    }
}

void publishData(const String& topic, const String& data) {
    String path = _deviceRoot;
    path += topic;
    pushToQueue(path, data);
}

void publistWidget(const String& data) {
    String path = _deviceRoot;
    path += "config";
    pushToQueue(path, data);
}

void publishChart(const String& name, const String& data) {
    String path = _deviceRoot;
    path += name;
    path += "_ch";
    pushToQueue(path, data);
}

void publishControl(const String& deviceId, const String& objName, const String& data) {
    String path = _deviceRoot;
    path += objName;
    path += "/control";
    pushToQueue(path, data);
}

void publishState(const String& objName, const String& data) {
    String path = _deviceRoot;
    path += objName;
    path += "/status";
    pushToQueue(path, data);
}

void publishOrder(const String& deviceId, const String& data) {
    String path = _prefix;
    path += "/";
    path += deviceId;
    path += "/order";
    pushToQueue(path, data);
}

const String getStateStr() {
    switch (_mqtt.state()) {
        case -4:
            return F("no respond");
            break;
        case -3:
            return F("connection was broken");
            break;
        case -2:
            return F("connection failed");
            break;
        case -1:
            return F("client disconnected");
            break;
        case 0:
            return F("client connected");
            break;
        case 1:
            return F("doesn't support the requested version");
            break;
        case 2:
            return F("rejected the client identifier");
            break;
        case 3:
            return F("unable to accept the connection");
            break;
        case 4:
            return F("wrong username/password");
            break;
        case 5:
            return F("not authorized to connect");
            break;
        default:
            return F("unspecified");
            break;
    }
}
}  // namespace MqttClient