#pragma once

#include "Base/Collection.h"

#include "Base/Sensor.h"

extern Collection<Sensor> sensors;

namespace Sensors {
void update();
}