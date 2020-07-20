#pragma once

#include "Objects/OneWireBus.h"
#include "Sensors/ADCSensor.h"
#include "Sensors/DallasSensor.h"
#include "Sensors/DHTSensor.h"
#include "Sensors/BMP280Sensor.h"
#include "Sensors/BME280Sensor.h"
#include "Sensors/UltrasonicSensor.h"

enum class SensorType_t {
    ADC,
    DALLAS
};

namespace Sensors {

Sensor* add(SensorType_t type, const String& name, const String& assign);

void update();
}  // namespace Sensors
