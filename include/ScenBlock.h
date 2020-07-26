#pragma once

#include <Arduino.h>

enum class EquationSign : uint8_t {
    EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_OR_EQAL,
    GREATER,
    GREATER_OR_EQAL
};

class ParamItem {
   public:
    ParamItem(const String&);
    virtual ~ParamItem() = default;
    virtual const String value();

   protected:
    String _value;
};

class LiveParam : public ParamItem {
   public:
    LiveParam(const String& value) : ParamItem(value){};
    const String value() override;
};

class ScenBlock {
   public:
    ScenBlock(const String& condition, const String& commands);
    ~ScenBlock();
    bool enable(bool enabled);
    bool isEnabled();
    bool checkCondition(const String& object, const String& value);
    const String getCommands();

   private:
    ParamItem* createParam(const String& str);
    bool parseCondition(const String& str);
    bool parseSign(const String& str, EquationSign& sign);

   private:
    bool _valid;
    bool _enabled;
    String _commands;
    String _key;
    EquationSign _sign;
    ParamItem* _param;
};
