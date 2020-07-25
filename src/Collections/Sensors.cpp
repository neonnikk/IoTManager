#include "Collection/Sensors.h"

#include "Sensors/ADCSensor.h"
#include "Sensors/DallasSensor.h"
#include "Sensors/DHTSensor.h"
#include "Sensors/BMP280Sensor.h"
#include "Sensors/BME280Sensor.h"
#include "Sensors/UltrasonicSensor.h"

static const char* MODULE = "Sensors";

Collection<Sensor> sensors;

enum SensorType_t {
    ADC,
    DALLAS,
    ULTRASONIC,
    NUM_SENSOR_TYPES
};

#define WRONG_SENSOR_TYPE NUM_SENSOR_TYPES

static const char* sensorTypeStr[NUM_SENSOR_TYPES] = {"adc", "dallas", "ultrasonic"};

SensorType_t getSensorType(const String& typeStr) {
    size_t i = 0;
    for (i = 0; i < NUM_SENSOR_TYPES; i++) {
        if (typeStr.startsWith(sensorTypeStr[i])) {
            break;
        }
    }
    return SensorType_t(i);
};

template <>
Sensor* Collection<Sensor>::add(const String& name, const String& assign) {
    Sensor* item = NULL;
    SensorType_t type = getSensorType(name);
    switch (type) {
        case SensorType_t::ADC:
            item = new ADCSensor{name, assign};
            break;
        case SensorType_t::DALLAS:
            if (!onewire.attached()) {
                pm.error("attach bus first");
                break;
            }
            item = new DallasSensor{name, assign};
            break;
        case SensorType_t::ULTRASONIC:
            item = new UltrasonicSensor{name, assign};

        default:
            break;
    }
    if (item) {
        _list.push_back(item);
        return last();
    }
    return NULL;
}

namespace Sensors {
void update() {
    for (auto item : sensors.getItems()) {
        if (item->hasValue()) {
            String name = item->getName();
            String value = item->getValue();
            ValueType_t type = item->getValueType();
            pm.info(name + " : " + value + String(type));
            //runtime.write(name, value, type);
        }
    }
}
}  // namespace Sensors