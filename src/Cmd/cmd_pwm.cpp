#include "Cmd.h"

#include "Collection/Pwms.h"
#include "Collection/Widgets.h"

/*
* pwm №1 подключен к pin 12 состояние 100%
* pwm {id:1,descr:"Зеленый",pin:12,state:100%,page:"Лампа",order:4}
*/
void cmd_pwm() {
    ParamStore params{sCmd.next()};

    String name = getObjectName(TAG_PWM, params.read("id").c_str());
    String assign = params.read("pin");
    String descr = params.read("name");
    String page = params.read("page");
    String state = params.read("state");
    String order = params.read("order");
    String widget = params.read("widget", "range");

    auto item = (Pwm*)pwms.add(name, assign);

    item->setMap(1, 100, 0, 1023);
    item->setValue(state);

    runtime.writeAsInt(name, state);

    String templateOverride = sCmd.next();

    Widgets::createWidget(descr, page, order, widget, name, templateOverride);
}

void cmd_pwmSet() {
    String name = getObjectName(TAG_PWM, sCmd.next());
    String value = sCmd.next();

    auto* item = pwms.get(name);
    if (item) {
        item->setValue(value);
    }
    runtime.writeAsInt(name, value);
}