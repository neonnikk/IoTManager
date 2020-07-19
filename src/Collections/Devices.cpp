#include "Collection/Devices.h"

#include "Broadcast.h"
#include "Config.h"
#include "Utils/SysUtils.h"
#include "Utils/FileUtils.h"
#include "PrintMessage.h"

static const char* MODULE = "Devices";

namespace Devices {

std::vector<DeviceItem*> _list;

DeviceItem* find(const String& id) {
    for (size_t i = 0; i < _list.size(); i++) {
        if (id.equals(_list.at(i)->id())) {
            return _list.at(i);
        }
    }
    return NULL;
}

void get(String& res, unsigned long ttl_sec) {
    for (auto item : _list) {
        if (ttl_sec && (item->seenago() > ttl_sec)) {
            continue;
        }
        res += item->asString();
        res += "\r\n";
    }
}

void toCSV(const String filename) {
    auto file = LittleFS.open(filename, "w");
    if (!file) {
        pm.error("save: " + filename);
        return;
    }
    file.println("id;name;url;lastseen");

    for (auto* item : _list) {
        String data = item->asString();
        file.println(data);
    }
    file.close();
}

void add(const String& id, const String& name, const String& ip) {
    DeviceItem* item = find(id);
    if (item) {
        item->update(name, ip);
        pm.info("updated");
    } else {
        item = new DeviceItem(id, name, ip);
        _list.push_back(item);
        pm.info("added");
    }
}

size_t size() {
    return _list.size();
}

const String asJson() {
    size_t count = _list.size();
    size_t n = 1;
    String res = "{\"devices\":[";
    for (auto* item : _list) {
        res += "{\"id\":\"";
        res += item->id();
        res += "\",";
        res += "\"name\":\"";
        res += item->name();
        res += "\",";
        res += "\"url\":\"";
        res += "<a href='http://";
        res += item->ip().toString();
        res += "' target='_blank'>";
        res += item->ip().toString();
        res += "</a>";
        res += "\",";
        res += "\"lastseen\":";
        res += item->seenago();
        res += (n++ < count ? "}," : "}");
    }
    res += "]}";
    return res;
};

}  // namespace Devices