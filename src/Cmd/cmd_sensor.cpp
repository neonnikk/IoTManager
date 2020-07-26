#include "Cmd.h"

#include "Collection/Sensors.h"
#include "Collection/Widgets.h"

static const char *MODULE = TAG_SENSOR;

void cmd_sensor() {
    ParamStore params{sCmd.next()};
    String templateOverride{sCmd.next()};

    Serial.println(params.get(TAG_NAME));

    String objName = getObjectName(params.get(TAG_NAME), params.get(TAG_ID));
    String assign = params.get(TAG_PIN);

    auto item = sensors.add(objName, assign);
    if (!item) {
        pm.error("on add: " + objName);
        return;
    }

    item->setReadInterval(parsePeriod(params.get("refresh", "5s")));

    auto mapper = createMapper(params.get("map"));
    if (mapper) {
        item->setMap(mapper);
    }

    // TODO
    String filter = params.get("filter");

    Widgets::createWidget(objName, params, "anydata", templateOverride);
}