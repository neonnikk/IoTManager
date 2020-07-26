#include "Cmd.h"

#include "Collection/Devices.h"

static const char *MODULE = "cmd_get";

void cmd_get() {
    String obj = sCmd.next();
    String param = sCmd.next();
    String res = "";

    if (!obj.isEmpty()) {
        if (obj.equalsIgnoreCase("state")) {
            if (param.isEmpty()) {
                res = runtime.asJson();
            } else {
                res = runtime.get(param.c_str());
            }
        } else if (obj.equalsIgnoreCase("devices")) {
            Devices::get(res, param.toInt());
        } else {
            res = F("unknown param");
        }
    } else {
        res = F("unknown obj");
    }
    pm.info(res);
    if (term) {
        term->println(res.c_str());
    }
}
