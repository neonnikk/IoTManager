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
size_t _lastSize = 0;
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

bool pushToQueue(const String& topic, const String& data) {
    if (_queue.size() > 32) {
        pm.error("queue full");
        return false;
    }
    _queue.push_back(MqttMessage{topic.c_str(), data.c_str()});
    return true;
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
        size_t grow = _queue.size() > _lastSize ? _queue.size() - _lastSize : 0;
        if (grow) {
            grow = grow > 8 ? 8 : grow;
        } else {
            grow = 1;
        }

        while (grow) {
            auto message = _queue.front();
            if (message.isValid()) {
                _mqtt.publish(message.getTopic(), message.getData());
            }
            _queue.pop_front();
            grow--;
        }
    }
    _lastSize = _queue.size();
}

bool extractObj(const String& str, String& objType, int& objId) {
    size_t i;
    for (i = str.length() - 1; i > 0; i--) {
        if (!isDigit(str[i])) break;
    }
    objType = str.substring(0, i + 1);
    objId = str.substring(i + 1).toInt();
    return !objType.isEmpty() && objId > 0;
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
    } else if (topicStr.endsWith("control")) {
        String objType;
        int objId;
        if (extractObj(topicStr.substring(_deviceRoot.length(), topicStr.lastIndexOf("/")), objType, objId)) {
            addCommand(objType + "Set " + objId + " " + payloadStr);
        } else {
            pm.error("wrong control");
        }
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

bool publishChart(const String& name, const String& data) {
    String path = _deviceRoot;
    path += name;
    path += "_ch";
    path += "/status";

    String buf = "{\"status\":[" + data + "]}";
    return pushToQueue(path, data);
}

void publishControl(const String& deviceId, const String& objName, const String& data) {
    String path = _deviceRoot;
    path += objName;
    path += "/control";
    pushToQueue(path, data);
}

bool publishState(const String& objName, const String& data) {
    String path = _deviceRoot;
    path += objName;
    path += "/status";
    return pushToQueue(path, data);
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