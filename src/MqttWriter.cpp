#include "MqttWriter.h"

#include "PrintMessage.h"
#include "Utils/StringUtils.h"

static const char* MODULE = "MqttWriter";

MqttWriter::MqttWriter(PubSubClient* client, const String& topic) : _client{client},
                                                                    _topic{topic},
                                                                    _retain{false},
                                                                    _qos{0} {};

bool MqttWriter::begin() {
    if (!_client) {
        pm.error("client is null");
        return false;
    }
    if (!_client->connected()) {
        pm.error("not connected");
        return false;
    }
    return true;
}

bool MqttWriter::write(const String& data, size_t length) {
    _length = length;
    if (_client->beginPublish(_topic.c_str(), _length, _retain)) {
        if (!_client->write((uint8_t*)data.c_str(), _length)) {
            pm.error("on write :" + prettyBytes(_length) + ", payload: " + String(data));
            return false;
        }
        return _client->endPublish();
    }
    return false;
}

void MqttWriter::end() {
}