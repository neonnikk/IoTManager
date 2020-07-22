#include "Scenario.h"

#include "Global.h"
#include "Cmd.h"

#include "Base/KeyValue.h"
#include "Base/StringQueue.h"

#include "ScenBlock.h"

static const char* MODULE = "Scenario";

namespace Scenario {

std::vector<ScenBlock*> _items;
StringQueue _events;
bool _ready = false;

void process(KeyValue* obj) {
    process(obj->getKey());
}

void process(const String name) {
    if (config.general()->isScenarioEnabled()) {
        _events.push(name);
    }
}

bool isBlockEnabled(size_t num) {
    return _items.at(num)->isEnabled();
}

void enableBlock(size_t num, boolean value) {
    _items.at(num)->enable(value);
}

void reinit() {
    _ready = false;
}

bool extractBlock(const String& buf, size_t& startIndex, String& block) {
    int endIndex = buf.indexOf("end", startIndex);
    if (endIndex < 0) {
        return false;
    }
    block = buf.substring(startIndex, endIndex);
    startIndex = endIndex + 4;
    return true;
}

const String removeComments(const String buf) {
    String res = "";
    size_t startIndex = 0;
    while (startIndex < buf.length() - 1) {
        int endIndex = buf.indexOf("\n", startIndex);
        String line = buf.substring(startIndex, endIndex);
        startIndex = endIndex + 1;
        if (!line.startsWith("//")) {
            res += line + "\n";
        }
    }
    return res;
}

void init() {
    _items.clear();
    String buf;
    if (!readFile(DEVICE_SCENARIO_FILE, buf)) {
        config.general()->enableScenario(false);
        return;
    }
    if (buf.isEmpty()) {
        return;
    }
    buf += "\n";
    buf.replace("\r\n", "\n");
    buf.replace("\r", "\n");

    buf = removeComments(buf);

    size_t pos = 0;
    while (pos < buf.length() - 1) {
        String item;
        if (!extractBlock(buf, pos, item)) {
            break;
        }
        if (!item.isEmpty()) {
            _items.push_back(new ScenBlock(item));
        }
    }

    pm.info("items: " + String(_items.size(), DEC));
}

void loop() {
    if (!_ready) {
        _ready = true;
        init();
        return;
    }
    if (!_events.available()) {
        return;
    }
    String event;
    _events.pop(event);
    if (event.isEmpty()) {
        return;
    }
    String value = runtime.read(event);
    for (auto item : _items) {
        if (item->isEnabled()) {
            if (item->checkCondition(event, value)) {
                addCommands(item->getCommands());
            }
        }
    }
}

}  // namespace Scenario
