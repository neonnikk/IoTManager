#include "Scenario.h"

#include "Global.h"
#include "Cmd.h"

#include "Base/KeyValue.h"
#include "Base/StringQueue.h"

#include "ScenBlock.h"

static const char* MODULE = "Scenario";

namespace Scenario {

std::vector<ScenBlock*> _items;

std::list<KeyValue*> _events;

bool _ready = false;

void process(KeyValue* obj) {
    if (config.general()->isScenarioEnabled()) {
        _events.push_back(obj);
    }
}

bool isBlockEnabled(size_t num) {
    return _items.at(num)->isEnabled();
}

void enableBlock(size_t num, boolean value) {
    _items.at(num)->enable(value);
}

void clear() {
    for (auto item : _items) {
        delete item;
    }
    _items.clear();
}

void load() {
    auto file = LittleFS.open(DEVICE_SCENARIO_FILE, FILE_READ);
    if (!file) {
        pm.error("open " + String(DEVICE_SCENARIO_FILE));
        return;
    }
    String condition = "";
    String commands = "";
    bool in_block = false;
    while (file.available()) {
        String line = file.readStringUntil('\n');
        if (line.startsWith("//")) {
            continue;
        }
        if (in_block) {
            if (line.startsWith("end")) {
                _items.push_back(new ScenBlock(condition, commands));
                condition = "";
                commands = "";
                in_block = false;
                continue;
            }
            commands += line;
            commands += '\n';
        } else {
            condition = line;
            in_block = true;
        }
    }
    file.close();
}

void init() {
    clear();

    load();

    if (_items.size()) {
        pm.info("items: " + String(_items.size()));
    } else {
        pm.info("disabled");
        config.general()->enableScenario(false);
    }
}

void loop() {
    if (!config.general()->isScenarioEnabled()) {
        return;
    }
    return;
    if (!_events.size()) {
        return;
    }

    Serial.printf("events: %d", _events.size());
    KeyValue* kv = _events.front();
    _events.pop_front();
    for (auto item : _items) {
        if (item->isEnabled()) {
            if (item->checkCondition(kv->getKey(), kv->getValue())) {
                addCommands(item->getCommands());
            } else {
                Serial.printf("%s: %s\t", kv->getKey(), kv->getValue());
            }
        }
    }
}

}  // namespace Scenario
