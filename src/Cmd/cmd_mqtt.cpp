#include "Cmd.h"

#include "MqttClient.h"

void cmd_mqtt() {
    String topic = sCmd.next();
    String data = sCmd.next();

    MqttClient::publishOrder(topic, data);
}
