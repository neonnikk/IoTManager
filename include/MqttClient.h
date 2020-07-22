#pragma once

#include <Arduino.h>

#include "MqttWriter.h"

namespace MqttClient {

MqttWriter* getWriter(const char* topic);

void init();

bool isConnected();

bool connect();

void reconnect();

void loop();

void subscribe();

void publishData(const String& topic, const String& data);

bool publishChart(const String& name, const String& data);

void publistWidget(const String& data);

void publishControl(const String& id, const String& topic, const String& state);

bool publishState(const String& topic, const String& data);

void publishOrder(const String& topic, const String& data);

const String getStateStr();

}  // namespace MqttClient
