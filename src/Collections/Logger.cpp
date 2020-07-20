#include "Collection/Logger.h"

#include "Objects/LoggerTask.h"

#include "Base/Writer.h"
#include "MqttClient.h"

#include "Utils/StringUtils.h"

#include <LITTLEFS.h>

namespace Logger {
std::vector<LoggerTask*> _list;

const String asJson() {
    String res = "{\"logs\":[";
    for (size_t i = 0; i < _list.size(); i++) {
        res += _list.at(i)->asJson();
        if (i < _list.size() - 1) res += ",";
    }
    res += "]}";
    return res;
}

void asCSV(const String& filename) {
    auto file = LittleFS.open(filename.c_str(), FILE_WRITE);
    file.println(F("№ п/п;Наименование;Записей;Размер;C;По"));
    for (auto item : _list) {
        file.println(item->asCVS());
    }
    file.close();
}

void forEach(LoggerTaskHandler func) {
    for (auto item : _list) {
        func(item);
    }
}

void update() {
    for (auto item : _list) {
        item->update();
    }
}

void add(const char* name, unsigned long interval, size_t storeLimit) {
    size_t id = _list.size() + 1;
    LoggerTask* item = new LoggerTask{id, name, interval, storeLimit};
    _list.push_back(item);
}

void init() {
    _list.clear();
}

void clear() {
    for (size_t i = 0; i < _list.size(); i++) {
        _list.at(i)->clear();
    }
}

}  // namespace Logger