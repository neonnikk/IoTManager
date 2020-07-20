#pragma once

#include <Arduino.h>

#include "Base/Sensor.h"
#include "Dallas.h"
#include "Utils/TimeUtils.h"
#include "PrintMessage.h"

class DallasSensor : public Sensor,
                     public OneWireAddressAssigned {
   public:
    DallasSensor(const String& name, const String& assign) : Sensor(name, assign, VT_FLOAT),
                                                             OneWireAddressAssigned{this} {
        _state = DallasSensorState::IDLE;
        _obj = new Dallas(getAddress());
        if (!_obj->isConnected()) {
            Serial.print("not found: ");
            Serial.println(getName());
        }
    };

    ~DallasSensor() {
        delete _obj;
    }

    bool sensorReady() override {
        bool res = false;
        switch (_state) {
            case DallasSensorState::IDLE:
                if (_obj->requestTemperature()) {
                    _state = DallasSensorState::CONVERSATION;
                    _requestConversationTime = millis();
                    res = false;
                }
                break;
            case DallasSensorState::CONVERSATION:
                if (millis_since(_requestConversationTime) >= _obj->getWaitTimeForConversion()) {
                    if (_obj->isConversionComplete()) {
                        _value = _obj->getTempC();
                        _state = DallasSensorState::IDLE;
                        res = true;
                    }
                };
                break;
            default:
                break;
        };
        return res;
    }

    float readSensor() override {
        return _value;
    }

   private:
    enum class DallasSensorState {
        IDLE,
        CONVERSATION
    };
    float _value;
    unsigned long _requestConversationTime;
    DallasSensorState _state;
    Dallas* _obj;
};
