#pragma once

#include <Arduino.h>

#include "Base/KeyValue.h"
#include "Base/StringQueue.h"

enum class EquationSign : uint8_t {
    OP_EQUAL,
    OP_NOT_EQUAL,
    OP_LESS,
    OP_LESS_OR_EQAL,
    OP_GREATER,
    OP_GREATER_OR_EQAL
};

class ParamItem {
   public:
    ParamItem(const String&);
    virtual ~ParamItem();

    virtual const char* value();

   protected:
    char* _value;
};

class LiveParam : public ParamItem {
   public:
    LiveParam(const String& value) : ParamItem(value){};
    const char* value() override;
};

class ScenBlock {
   public:
    ScenBlock(const String& str);
    ~ScenBlock();
    bool process(const String& value);
    bool enable(bool enabled);
    bool isEnabled();

   private:
    bool equation(const String& object, const String& value);
    bool parseCondition(const String& str);
    bool parseSign(const String& str, EquationSign& sign);

   private:
    bool _valid;
    bool _enabled;
    String _commands;
    String _obj;
    EquationSign _sign;
    ParamItem* _param;
};

namespace Scenario {

void process(KeyValue*);

void process(const String);

void reinit();

void init();

void loop();

void enableBlock(size_t num, boolean enable);

bool isBlockEnabled(size_t num);

}  // namespace Scenario