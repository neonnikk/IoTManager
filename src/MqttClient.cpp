#include "MqttClient.h"

#include <ESP.h>
#include <PubSubClient.h>
#include <WiFiClient.h>

#include "Cmd.h"
#include "Runtime.h"
#include "Config/MqttConfig.h"
#include "Collection/Logger.h"
#include "Utils/TimeUtils.h"

static const char* MODULE = "Mqtt";

namespace MqttClient {

struct MqttMessage {
   private:
    String _topic;
    String _data;

   public:
    MqttMessage(const String& topic, const String& data) : _topic{topic}, _data{data} {}

    const String getTopic() {
        return _topic;
    }
    const String getData() {
        return _data;
    }
};

WiFiClient espClient;
PubSubClient _mqtt(espClient);

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
    String path = _deviceRoot + "/" + topic;
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
    _deviceRoot = _prefix + "/" + _uuid;
}

bool connect() {
    init();
    bool res = false;
    _mqtt.setServer(_addr.c_str(), _port);
    res = _mqtt.connect(_uuid.c_str(), _user.c_str(), _pass.c_str());
    if (res) {
        pm.info("connected " + _addr + ":" + String(_port, DEC));
        subscribe();
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
    if (_mqtt.connected()) {
        _mqtt.disconnect();
    }
    init();
    connect();
}

void subscribe() {
    pm.info("subscribe: " + _prefix);
    _mqtt.setCallback(handleSubscribedUpdates);
    _mqtt.subscribe(_prefix.c_str());
    _mqtt.subscribe((_deviceRoot + "/+/control").c_str());
    _mqtt.subscribe((_deviceRoot + "/order").c_str());
    _mqtt.subscribe((_deviceRoot + "/update").c_str());
    _mqtt.subscribe((_deviceRoot + "/devc").c_str());
    _mqtt.subscribe((_deviceRoot + "/devs").c_str());
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
    _queue.push_back(MqttMessage(topic.c_str(), data.c_str()));
}

bool isConnected() {
    return _mqtt.connected();
}

bool hasAttempts() {
    return RECONNECT_ATTEMPTS_MAX ? _connectionAttempts < RECONNECT_ATTEMPTS_MAX : true;
}

void loop() {
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
        reconnect();
        _lastConnetionAttempt = millis();
        if (!isConnected()) {
            _connectionAttempts++;
            if (!hasAttempts()) {
                config.mqtt()->enable(false);
            }
            return;
        }
    }

    if (!_queue.empty()) {
        auto* writer = new MqttWriter(&_mqtt, _queue.front().getTopic());
        writer->begin();
        writer->write(_queue.front().getData(), _queue.front().getData().length());
        writer->end();
        _queue.pop_front();
        delete writer;
    }

    _mqtt.loop();
}

const String parseControl(const String& str) {
    String res;
    String num1 = str.substring(str.length() - 1);
    String num2 = str.substring(str.length() - 2, str.length() - 1);
    if (isDigitStr(num1) && isDigitStr(num2)) {
        res = str.substring(0, str.length() - 2) + "Set" + num2 + num1;
    } else {
        if (isDigitStr(num1)) {
            res = str.substring(0, str.length() - 1) + "Set" + num1;
        }
    }
    return res;
}

void handleSubscribedUpdates(char* topic, uint8_t* payload, size_t length) {
    pm.info("<= " + String(topic));
    String topicStr = String(topic);
    String payloadStr = "";
    payloadStr.reserve(length + 1);
    for (size_t i = 0; i < length; i++) {
        payloadStr += (char)payload[i];
    }
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
    path += "/";
    path += topic;
    pushToQueue(path, data);
}

void publistWidget(const String& data) {
    String path = _deviceRoot;
    path += "/config";
    pushToQueue(path, data);
}

void publishChart(const String& name, const String& data) {
    String path = _deviceRoot;
    path += "/";
    path += name;
    path += "_ch";
    pushToQueue(path, data);
}

void publishControl(const String& deviceId, const String& objName, const String& data) {
    String path = _prefix;
    path += "/";
    path += deviceId;
    path += "/";
    path += objName;
    path += "/control";
    pushToQueue(path, data);
}

void publishState(const String& objName, const String& data) {
    String path = _deviceRoot;
    path += "/";
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