#include "Web.h"

#include "Objects/I2CScanner.h"
#include "Objects/OneWireScanner.h"
#include "Actions.h"
#include "Global.h"
#include "Runtime.h"
#include "Collection/Logger.h"
#include "HttpServer.h"

static const char* MODULE = "Web";

namespace Web {

void init() {
    // dnsServer.start(53, "*", WiFi.softAPIP());
    // server.addHandler(new CaptiveRequestHandler(jsonReadStr(configSetupJson, "name").c_str())).setFilter(ON_AP_FILTER);

    server.on("/restart", HTTP_GET, [](AsyncWebServerRequest* request) {
        if (request->hasArg("device")) {
            if (request->getParam("device")->value() == "ok") {
                Actions::execute(ACT_SYSTEM_REBOOT);
                request->send(200);
            }
        };
    });

    server.on(
        "/config", HTTP_GET, [](AsyncWebServerRequest* request) {
            if (request->hasArg("add")) {
                add_to_config(request->getParam("add")->value());
                request->redirect(PAGE_SETUP);
                return;
            }
        });

    server.on(
        "/set", HTTP_GET, [](AsyncWebServerRequest* request) {
            if (request->hasArg("preset")) {
                load_device_preset(request->getParam("preset")->value().toInt());
                request->redirect(PAGE_SETUP);
                return;
            }
            if (request->hasArg("devinit")) {
                load_device_config();
                request->send(200);
                return;
            }
            if (request->hasArg("sceninit")) {
                Scenario::init();
                request->send(200);
                return;
            }
            if (request->hasArg("refreshLogs")) {
                Actions::execute(ACT_LOGGER_REFRESH);
                request->redirect(PAGE_UTILITIES);
                return;
            }
            if (request->hasArg(TAG_CHECK_MQTT)) {
                DynamicJsonBuffer json;
                JsonObject& root = json.createObject();
                root["title"] = "<button class=\"close\" onclick=\"toggle('my-block')\">×</button>" + MqttClient::getStateStr();
                root["class"] = "pop-up";
                String buf;
                root.printTo(buf);
                request->send(200, "text/html", buf);
                return;
            }
            if (request->hasArg(TAG_SHARE_MQTT)) {
                Actions::execute(ACT_MQTT_BROADCAST);
                request->send(200);
                return;
            }
            if (request->hasArg(TAG_I2C)) {
                Actions::execute(ACT_BUS_SCAN, new I2CScanner());
                request->redirect(PAGE_UTILITIES);
                return;
            }
            if (request->hasArg(TAG_ONE_WIRE)) {
                Actions::execute(ACT_BUS_SCAN, new OneWireScanner());
                request->redirect(PAGE_UTILITIES);
                return;
            }

            for (size_t i = 0; i < request->params(); i++) {
                String param = request->getParam(i)->name();
                String value = request->getParam(i)->value();
                pm.info(param + ": " + value);
                if (!config.setParamByName(param, value)) {
                    pm.error("unknown: " + param);
                    request->send(404);
                    return;
                }
            }
            request->send(200);
        });

    server.on("/check", HTTP_GET, [](AsyncWebServerRequest* request) {
        String msg = "";
        String lastVersion = runtime.get(TAG_LAST_VERSION);
        // Errors
        if (!FLASH_4MB) {
            msg = F("Обновление \"по воздуху\" не поддерживается!");
        } else if (!NetworkManager::isNetworkActive()) {
            msg = F("Устройство не подключено к роутеру!");
        } else if (lastVersion == "notsupported") {
            msg = F("Обновление возможно только через usb!");
        } else if (lastVersion == "error" || lastVersion.isEmpty()) {
            Actions::execute(ACT_UPDATES_CHECK);
            msg = F("Нажмите на кнопку \"обновить прошивку\" повторно...");
        } else {
            // TODO Версия должна быть выше
            if (lastVersion == FIRMWARE_VERSION) {
                msg = F("Версия прошивки актуальна.");
            } else if (lastVersion != FIRMWARE_VERSION) {
                msg = "Обновление: ";
                msg += lastVersion;
                msg += F("<a href =\"#\" class=\"btn btn-block btn-danger\" onclick=\"send_request(this, '/upgrade');setTimeout(function(){ location.href='/'; }, 120000);html('my-block','<span class=loader></span>Идет установка, по ее заверщению страница автоматически перезагрузится...')\">Установить</a>");
            }
        }
        DynamicJsonBuffer json;
        JsonObject& root = json.createObject();
        root["title"] = "<button class=\"close\" onclick=\"toggle('my-block')\">" + msg + "</button>";
        root["class"] = "pop-up";
        String buf;
        root.printTo(buf);
        request->send(200, "text/html", buf);
    });

    server.on("/upgrade", HTTP_GET, [](AsyncWebServerRequest* request) {
        Actions::execute(ACT_UPGRADE);
        request->send(200);
    });
}

}  // namespace Web