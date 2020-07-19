#include "Global.h"

#include "Collection/Widgets.h"
#include "Collection/Logger.h"

TickerScheduler ts(ANNOUNCE + 1);

WiFiClient wifiClient;
AsyncWebServer server{80};
AsyncWebSocket ws{"/ws"};
AsyncEventSource events{"/events"};

void config_save() {
    String buf;
    config.save(buf);
    writeFile(DEVICE_CONFIG_FILE, buf);
    config.setSynced();
}

void load_config() {
    String buf;
    if (readFile(DEVICE_CONFIG_FILE, buf)) {
        config.load(buf);
    }
}

void publishWidgets() {
    Widgets::forEach([](String json) {
        return MqttClient::publistWidget(json);
    });
}

void publishCharts() {
    Logger::forEach([](LoggerTask* task) {
        task->readEntries([](LogMetadata* meta, uint8_t* data) {
            String buf = "{\"status\":[";
            buf += LogEntry(data).asChartEntry();
            buf += "]}";
            return MqttClient::publishChart(meta->getName(), buf);
        });
        return true;
    });
}

void config_add(const String& str) {
    addFile(DEVICE_COMMAND_FILE, str);
}

void setPreset(size_t num) {
    copyFile(getConfigFile(num, CT_CONFIG), DEVICE_COMMAND_FILE);
    copyFile(getConfigFile(num, CT_SCENARIO), DEVICE_SCENARIO_FILE);
    device_init();
}

bool extractCommand(const String buf, size_t& startIndex, String& block) {
    int endIndex = buf.indexOf("\n", startIndex);
    if (endIndex < 0) {
        return false;
    }
    block = buf.substring(startIndex, endIndex);
    startIndex = endIndex + 1;
    return true;
}

void device_init() {
    Widgets::clear();
    String buf;
    if (!readFile(DEVICE_COMMAND_FILE, buf)) {
        return;
    }
    size_t pos = 0;
    while (pos < buf.length() - 1) {
        String item;
        if (!extractCommand(buf, pos, item)) {
            break;
        }
        if (!item.startsWith("//") && !item.isEmpty()) {
            ExecuteCommand(item);
        }
    }
}
