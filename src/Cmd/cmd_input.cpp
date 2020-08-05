#include "Cmd.h"

#include "Collection/Widgets.h"

void cmd_inputDigit() {
    ParamStore params{sCmd.next()};
    String temlateOverride{sCmd.next()};

    String objName = getObjectName(TAG_DIGITAL, params.get(TAG_ID));
    String value = params.get(TAG_VALUE);

    runtime.write(objName, value);
    Widgets::createWidget(objName, params, "inputNum", temlateOverride);
}

void cmd_inputTime() {
    ParamStore params{sCmd.next()};
    String temlateOverride{sCmd.next()};

    String objName = getObjectName(TAG_TIME, params.get(TAG_ID));
    String value = params.get(TAG_STATE);

    runtime.write(objName, value);
    Widgets::createWidget(objName, params, "inputTime", temlateOverride);
}

void cmd_digitSet() {
    ParamStore params{sCmd.next()};

    String objName = getObjectName(TAG_DIGIT, params.get(TAG_ID));
    String value = params.get(TAG_VALUE);

    runtime.writeAsInt(objName, value);
}

void cmd_timeSet() {
    ParamStore params{sCmd.next()};

    String objName = getObjectName(TAG_TIME, params.get(TAG_ID));
    String value = params.get(TAG_VALUE);

    runtime.write(objName, value);
}