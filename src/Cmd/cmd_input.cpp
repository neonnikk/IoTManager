#include "Cmd.h"

#include "Collection/Widgets.h"

void cmd_inputDigit() {
    String name = sCmd.next();
    String number = name.substring(5);
  
    String descr = sCmd.next();
    descr.replace("#", " ");
    String page_name = sCmd.next();
    page_name.replace("#", " ");
    String value = sCmd.next();
    int order = String(sCmd.next()).toInt();

    String objName = "digit" + number;

    runtime.writeAsInt(objName, value);

    // Widgets::createWidget(descr, page_name, order, "inputNum", objName);
}

void cmd_inputTime() {
    String name = sCmd.next();
    String number = name.substring(4);
    String widget_name = sCmd.next();
    widget_name.replace("#", " ");
    String page_name = sCmd.next();
    page_name.replace("#", " ");
    String state = sCmd.next();
    int order = String(sCmd.next()).toInt();

    String objName = "time" + name;

    runtime.write(objName, state);

//    Widgets::createWidget(widget_name, page_name, order, "inputTime", "time" + number);
}

void cmd_digitSet() {
    String name = sCmd.next();
    String value = sCmd.next();

    String objName = "digit" + name;

    runtime.writeAsInt(objName, value);
}

void cmd_timeSet() {
    String name = sCmd.next();
    String value = sCmd.next();

    String objName = "time" + name;

    runtime.write(objName, value);
}