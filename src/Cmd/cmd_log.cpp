#include "Cmd.h"

#include "Collection/Logger.h"
#include "Collection/Widgets.h"

static const char *MODULE = TAG_LOG;

// log {id:1,data:"adc1",descr:"Температура",page:"Датчики",order:2}
void cmd_log() {
    ParamStore params{sCmd.next()};
    String temlateOverride{sCmd.next()};

    int id = params.getInt(TAG_ID);
    if (!id) {
        pm.error(TAG_ID);
        return;
    }
    String data = params.get(TAG_DATA);
    if (!data) {
        pm.error(TAG_DATA);
        return;
    }
    unsigned long interval_ms = parsePeriod(params.get(TAG_INTERVAL, "5s"));
    unsigned long period_ms = parsePeriod(params.get(TAG_PERIOD, "1h"));

    Logger::add(id, data, interval_ms, period_ms);

    int maxCount = period_ms / interval_ms;
    params.write("maxCount", maxCount);

    Widgets::createWidget(data + "_ch", params, "chart", temlateOverride);
}