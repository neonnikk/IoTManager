#pragma once

#include <PubSubClient.h>

#include "Base/Writer.h"

class MqttWriter : public Writer {
   public:
    MqttWriter(PubSubClient* client, const String& topic);

    bool begin() override;
    bool write(const String& data, size_t length) override;
    void end() override;

   private:
    PubSubClient* _client;
    String _topic;
    bool _retain;
    uint8_t _qos;
    size_t _length;
};
