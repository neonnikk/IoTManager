#include "Runtime.h"

#include "MqttClient.h"
#include "Scenario.h"
#include "StringConsts.h"

Runtime runtime(DEVICE_RUNTIME_FILE);

void Runtime::onAdd(KeyValue* item) {
    //
}

void Runtime::onUpdate(KeyValue* item) {
    publishMqtt(item);
    fireEvent(item);
}

void Runtime::publish() {
    for (auto item : _items) {
        publishMqtt(item);
    }
}

void Runtime::publishMqtt(KeyValue* item) {
    if (item->isPublished()) {
        MqttClient::publishState(item->getKey(), item->asJson());
    }
}

void Runtime::fireEvent(KeyValue* item) {
    if (item->isEvent()) {
        Scenario::process(item);
    }
}
