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

// sensor {id:1,type:"adc",pin:0,refresh:"5s",name:"Аналоговый#вход,#%",map:"1-1023/1-100",page:"Датчики",order:1}
void cmd_sensor() {
    ParamStore params{sCmd.next()};

    SensorType_t type = getSensorType(params.read(TAG_TYPE));
    String nameStr = getObjectName(TAG_SENSOR, params.read(TAG_ID));
    String assignStr = params.read(TAG_PIN);

    auto *item = Sensors::add(type, nameStr, assignStr);
    if (!item) {
        pm.error("on add");
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

    String descr = params.read("name");
    String widget = params.read("widget", "any-data");
    String page = params.read("page");
    String order = params.read("order");

    Widgets::createWidget(descr, page, order, widget, nameStr);
}