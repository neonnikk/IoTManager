#include "Scenario.h"

#include "Global.h"
#include "Cmd.h"

static const char* MODULE = "Scenario";

ParamItem::ParamItem(const String& value) {
    _value = strdup(value.c_str());
}

ParamItem::~ParamItem() {
    delete _value;
}

const char* ParamItem::value() {
    return _value;
}

const char* LiveParam::value() {
    return runtime.read(_value).c_str();
}

ScenBlock::ScenBlock(const String& str) {
    size_t split = str.indexOf("\n");
    _commands = str.substring(split + 1, str.indexOf("\nend"));
    _valid = parseCondition(str.substring(0, split)) && !_commands.isEmpty();
}

ScenBlock::~ScenBlock() {
    delete _param;
}

const String ScenBlock::getCommands() {
    return _commands;
}

bool ScenBlock::checkCondition(const String& key, const String& value) {
    if (!key.equals(_key)) {
        return false;
    }
    bool res = false;
    switch (_sign) {
        case EquationSign::OP_EQUAL:
            res = value.equals(_param->value());
            break;
        case EquationSign::OP_NOT_EQUAL:
            res = !value.equals(_param->value());
            break;
        case EquationSign::OP_LESS:
            res = value.toFloat() < atof(_param->value());
            break;
        case EquationSign::OP_LESS_OR_EQAL:
            res = value.toFloat() <= atof(_param->value());
            break;
        case EquationSign::OP_GREATER:
            res = value.toFloat() > atof(_param->value());
            break;
        case EquationSign::OP_GREATER_OR_EQAL:
            res = value.toFloat() >= atof(_param->value());
            break;
        default:
            break;
    }
    return res;
}

bool ScenBlock::enable(bool value) {
    _enabled = value;
    return isEnabled();
}

bool ScenBlock::isEnabled() {
    return _valid & _enabled;
}

bool ScenBlock::parseCondition(const String& str) {
    size_t s1 = str.indexOf(" ");
    if (s1 < 0) {
        pm.error("wrong line");
        return false;
    }
    size_t s2 = str.indexOf(" ", s1 + 1);
    _key = str.substring(0, s1);
    if (_key.isEmpty()) {
        pm.error("wrong obj");
        return false;
    }
    String signStr = str.substring(s1 + 1, s2);
    if (!parseSign(signStr, _sign)) {
        pm.error("wrong sign");
        return false;
    };
    String paramStr = str.substring(s2 + 1);
    if (paramStr.isEmpty()) {
        pm.error("wrong param");
        return false;
    }
    if (paramStr.startsWith("digit") || paramStr.startsWith("time")) {
        _param = new LiveParam(paramStr);
    } else {
        _param = new ParamItem(paramStr);
    }
    return true;
}

bool ScenBlock::parseSign(const String& str, EquationSign& sign) {
    bool res = true;
    if (str.equals("=")) {
        sign = EquationSign::OP_EQUAL;
    } else if (str.equals("!=")) {
        sign = EquationSign::OP_NOT_EQUAL;
    } else if (str.equals("<")) {
        sign = EquationSign::OP_LESS;
    } else if (str.equals(">")) {
        sign = EquationSign::OP_GREATER;
    } else if (str.equals(">=")) {
        sign = EquationSign::OP_GREATER_OR_EQAL;
    } else if (str.equals("<=")) {
        sign = EquationSign::OP_LESS_OR_EQAL;
    } else {
        res = false;
    }
    return res;
}

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

bool extractBlock(const String buf, size_t& startIndex, String& block) {
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
        if (line.startsWith("//")) {
            continue;
        }
        res += line + "\n";
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
    _ready = true;
}

void loop() {
    if (!_ready) {
        init();
        _ready = true;
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
