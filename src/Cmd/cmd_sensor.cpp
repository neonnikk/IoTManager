#include "Cmd.h"

#include "Collection/Sensors.h"
#include "Collection/Widgets.h"

static const char *MODULE = TAG_SENSOR;

void cmd_sensor() {
    ParamStore params{sCmd.next()};
    String templateOverride{sCmd.next()};

    String objName = getObjectName(params.read(TAG_NAME), params.read(TAG_ID));
    String assign = params.read(TAG_PIN);

    auto item = sensors.add(objName, assign);
    if (!item) {
        pm.error("on add: " + objName);
        return;
    }

    item->setReadInterval(parsePeriod(params.read("refresh", "5s")));

    auto mapper = createMapper(params.read("map"));
    if (mapper) {
        item->setMap(mapper);
    }

    // TODO
    String filter = params.read("filter");

    Widgets::createWidget(objName, params, "anydata", templateOverride);
}