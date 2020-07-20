#include "Cmd.h"

void cmd_stepper() {
    String name = sCmd.next();
    String pin_step = sCmd.next();
    String pin_dir = sCmd.next();

    // String objName = "stepper" + name;
    // liveData.write(objName, pin_step + " " + pin_dir);

    // pinMode(pin_step.toInt(), OUTPUT);
    // pinMode(pin_dir.toInt(), OUTPUT);
}

void cmd_stepperSet() {
    String stepper_number = sCmd.next();
    String steps = sCmd.next();
    // options.write("steps" + stepper_number, steps);
    // String stepper_speed = sCmd.next();
    // String pin_step = selectToMarker(options.read("stepper" + stepper_number), " ");
    // String pin_dir = deleteBeforeDelimiter(options.read("stepper" + stepper_number), " ");
    // Serial.println(pin_step);
    // Serial.println(pin_dir);
    // if (steps.toInt() > 0) digitalWrite(pin_dir.toInt(), HIGH);
    // if (steps.toInt() < 0) digitalWrite(pin_dir.toInt(), LOW);
    // if (stepper_number == "1") {
    //     ts.add(
    //         STEPPER1, stepper_speed.toInt(), [&](void *) {
    //             int steps_int = abs(options.readInt("steps1") * 2);
    //             static int count;
    //             count++;
    //             String pin_step = selectToMarker(options.read("stepper1"), " ");
    //             digitalWrite(pin_step.toInt(), !digitalRead(pin_step.toInt()));
    //             yield();
    //             if (count > steps_int) {
    //                 digitalWrite(pin_step.toInt(), LOW);
    //                 ts.remove(STEPPER1);
    //                 count = 0;
    //             }
    //         },
    //         nullptr, true);
    // }

    // if (stepper_number == "2") {
    //     ts.add(
    //         STEPPER2, stepper_speed.toInt(), [&](void *) {
    //             int steps_int = abs(options.readInt("steps2") * 2);
    //             static int count;
    //             count++;
    //             String pin_step = selectToMarker(options.read("stepper2"), " ");
    //             digitalWrite(pin_step.toInt(), !digitalRead(pin_step.toInt()));
    //             yield();
    //             if (count > steps_int) {
    //                 digitalWrite(pin_step.toInt(), LOW);
    //                 ts.remove(STEPPER2);
    //                 count = 0;
    //             }
    //         },
    //         nullptr, true);
    // }
}
