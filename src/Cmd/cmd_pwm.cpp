#include "Cmd.h"

#include "Collection/Pwms.h"
#include "Collection/Widgets.h"

static const char* MODULE = TAG_PWM;

/*
* pwm {id:1,descr:"Зеленый",pin:12,state:100%,page:"Лампа",order:4}
*/
void cmd_pwm() {
    ParamStore params{sCmd.next()};
    String temlateOverride{sCmd.next()};

    String name = getObjectName(TAG_PWM, params.get(TAG_ID));
    String assign = params.get(TAG_PIN);
    int state = params.getInt(TAG_STATE, 0);

    auto item = pwms.add(name, assign);
    if (!item) {
        pm.error("on add: " + name);
        return;
    }
    auto mapper = createMapper(params.get("map"));
    if (mapper) {
        item->setMap(mapper);
    }
    item->setValue(state);

    runtime.write(name, state);
    Widgets::createWidget(name, params, "range", temlateOverride);
}

void cmd_pwmSet() {
    String name = getObjectName(TAG_PWM, sCmd.next());
    int state = String(sCmd.next()).toInt();

    auto item = pwms.get(name);
    if (!item) {
        pm.error("not found: " + name);
        return;
    }

    item->setValue(state);
    runtime.write(name, state);
}