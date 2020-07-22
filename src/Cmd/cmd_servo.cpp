#include "Cmd.h"

#include "Collection/Servos.h"
#include "Collection/Widgets.h"

static const char* MODULE = TAG_SERVO;

// servo {id:1,pin:13,state:50,map:"0-100/0-180",descr:"Cервопривод",page:"Сервоприводы",order:2}
void cmd_servo() {
    ParamStore params{sCmd.next()};
    String temlateOverride{sCmd.next()};

    String objId = getObjectName(TAG_SERVO, params.read(TAG_ID));
    String assign = params.read(TAG_PIN);
    int state = params.readInt(TAG_STATE, 0);

    auto item = servos.add(objId, assign);
    if (!item) {
        pm.error("on add: " + objId);
        return;
    }
    auto mapper = createMapper(params.read("map"));
    if (mapper) {
        item->setMap(mapper);
    }
    item->setValue(state);

    runtime.write(objId, state);
    params.write("min", mapper->getParams().out.min);
    params.write("max", mapper->getParams().out.max);
    params.write("k", 1);

    Widgets::createWidget(objId, params, "range", temlateOverride);
}

void cmd_servoSet() {
    String id = sCmd.next();
    int value = String(sCmd.next()).toInt();

    String objId = TAG_SERVO + id;

    auto* item = servos.get(objId);
    if (!item) {
        return;
    }
    item->setValue(value);
    runtime.write(objId, value);
}
