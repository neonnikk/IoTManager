#include "Cmd.h"

#include "Collection/Sensors.h"
#include "Collection/Widgets.h"

static const char *MODULE = TAG_SENSOR;

struct MapParams {
    long in_min, in_max, out_min, out_max;
};

bool parseMapParams(const String &str, MapParams &p) {
    // TODO
    return false;
}

static const char *sensorTypeStr[NUM_SENSOR_TYPES] = {"adc", "dallas"};
SensorType_t getSensorType(const String &typeStr) {
    size_t i = 0;
    for (i = 0; i < NUM_SENSOR_TYPES; i++) {
        if (typeStr.equals(sensorTypeStr[i])) {
            break;
        }
    }
    return SensorType_t(i);
};

// sensor {id:1,name:"adc",pin:0,refresh:"5s",descr:"Аналоговый#вход,#%",map:"1-1023/1-100",page:"Датчики",order:1}
// sensor {id:1,name:"dallas",pin:"0x150x150x150x150x15",refresh:"5s",descr:"Температура",map:"1-1023/1-100",page:"Датчики",order:1}
void cmd_sensor() {
    ParamStore params{sCmd.next()};
    String temlateOverride{sCmd.next()};

    SensorType_t type = getSensorType(params.read(TAG_NAME));
    String name = getObjectName(params.read(TAG_NAME), params.read(TAG_ID));
    String assign = params.read(TAG_PIN);

    auto *item = Sensors::add(type, name, assign);
    if (!item) {
        pm.error("on add: " + name);
        return;
    }
    String mapStr = params.read("map");
    if (!mapStr.isEmpty()) {
        MapParams p;
        if (parseMapParams(mapStr, p)) {
            item->setMap(p.in_min, p.in_max, p.out_min, p.out_max);
        }
    }
    // TODO
    int refresh = parsePeriod(params.read("refresh", "5s"));
    // TODO
    String filter = params.read("filter");

    Widgets::createWidget(name, params, "anydata", temlateOverride);
}