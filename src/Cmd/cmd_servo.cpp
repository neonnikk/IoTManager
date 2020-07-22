#include "Cmd.h"

#include "Collection/Servos.h"
#include "Collection/Widgets.h"

void cmd_servoSet() {
    String name = sCmd.next();
    String value = sCmd.next();

    String objName = "servo" + name;

    auto* item = servos.get(objName);
    if (!item) {
        return;
    }
    item->setValue(value);

    runtime.writeAsInt(objName, value);
}

void cmd_servo() {
    //servo 1 13 50 Cервопривод Сервоприводы 0 100 0 180 2
    String name = sCmd.next();
    String pin = sCmd.next();
    String value = sCmd.next();

    String descr = sCmd.next();
    String page = sCmd.next();

    String min_value = sCmd.next();
    String max_value = sCmd.next();
    String min_deg = sCmd.next();
    String max_deg = sCmd.next();

    int order = String(sCmd.next()).toInt();

    servos.add(name, pin, value, min_value, max_value, min_deg, max_deg);

    // options.write("servo_pin" + name, pin);
    // value = map(value, min_value, max_value, min_deg, max_deg);
    // servo->write(value);
    // options.writeInt("s_min_val" + name, min_value);
    // options.writeInt("s_max_val" + name, max_value);
    // options.writeInt("s_min_deg" + name, min_deg);
    // options.writeInt("s_max_deg" + name, max_deg);
    // liveData.writeInt("servo" + name, value);

    // Widgets::createWidget(descr, page, order, "range", "servo" + name);
    // , "min", String(min_value), "max", String(max_value), "k", "1");
}