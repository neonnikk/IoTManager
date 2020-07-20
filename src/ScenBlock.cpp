#include "ScenBlock.h"

#include "Runtime.h"
#include "PrintMessage.h"
#include "StringConsts.h"

static const char* MODULE = "ScenBlock";

ParamItem::ParamItem(const String& value) {
    _value = strdup(value.c_str());
}

ParamItem::~ParamItem() {
    free(_value);
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
    if (_param) {
        delete _param;
    }
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

    if (!parseSign(str.substring(s1 + 1, s2), _sign)) {
        pm.error("wrong sign");
        return false;
    };

    _param = createParam(str.substring(s2 + 1));
    if (!_param) {
        pm.error("wrong param");
        return false;
    }

    return true;
}

ParamItem* ScenBlock::createParam(const String& str) {
    ParamItem* res = NULL;
    if (!str.isEmpty()) {
        if (str.startsWith(TAG_DIGIT) || str.startsWith(TAG_TIME)) {
            res = new LiveParam(str);
        } else {
            res = new ParamItem(str);
        }
    }
    return res;
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