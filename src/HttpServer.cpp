#include "HttpServer.h"

#include "Cmd.h"
#include "FSEditor.h"
#include "StringConsts.h"
#include "Config.h"

#include "Collection/Logger.h"
#include "Collection/Devices.h"
#include "Objects/OneWireBus.h"

#include "Utils/FileUtils.h"
#include "Utils/WebUtils.h"

AsyncWebServer server{80};
// AsyncWebSocket ws{"/ws"};
// AsyncEventSource events{"/events"};

namespace HttpServer {

static const char *MODULE = "Http";

void init() {
    String login = config.web()->getLogin();
    String pass = config.web()->getPass();
#ifdef ESP32
    server.addHandler(new FSEditor(LittleFS, login, pass));
#else
    server.addHandler(new FSEditor(login, pass));
#endif

    server.serveStatic("/css/", LittleFS, "/css/").setCacheControl("max-age=600");
    server.serveStatic("/js/", LittleFS, "/js/").setCacheControl("max-age=600");
    server.serveStatic("/favicon.ico", LittleFS, "/favicon.ico").setCacheControl("max-age=600");
    server.serveStatic("/icon.jpeg", LittleFS, "/icon.jpeg").setCacheControl("max-age=600");
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.htm").setAuthentication(login.c_str(), pass.c_str());

    server.onNotFound([](AsyncWebServerRequest *request) {
        pm.error("not found: " + getRequestInfo(request));
        request->send(404);
    });

    server.onFileUpload([](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
        // TODO
        if (!index) {
            pm.info("start upload " + filename);
        }
        if (final) {
            pm.info("finish upload: " + prettyBytes(index + len));
        }
    });

    server.on("/runtime.json", HTTP_GET, [](AsyncWebServerRequest *request) {
        String buf;
        request->send(200, "application/json", runtime.asJson());
    });

    server.on("/device.json", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "application/json", Devices::asJson());
    });

    server.on("/onewire.json", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "application/json", oneWire.asJson());
    });

    server.on("/logs.json", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "application/json", Logger::asJson());
    });

    server.on("/cmd", HTTP_GET, [](AsyncWebServerRequest *request) {
        String cmdStr = request->getParam("command")->value();
        addCommand(cmdStr);
        request->send(200);
    });

    server.on("/add", HTTP_GET, [](AsyncWebServerRequest *request) {
        String cmdStr = request->getParam("command")->value();
        addCommand(cmdStr);
        request->redirect(PAGE_SETUP);
    });

    server.begin();

    initOTA();
    initMDNS();
    initWS();
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        pm.info("connect: " + String(server->url()) + " " + String(client->id(), DEC));
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
    } else if (type == WS_EVT_ERROR) {
        Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
    } else if (type == WS_EVT_PONG) {
        Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
    } else if (type == WS_EVT_DATA) {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        String msg = "";
        if (info->final && info->index == 0 && info->len == len) {
            Serial.printf("ws[%s][%u] %s [%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);
            if (info->opcode == WS_TEXT) {
                for (size_t i = 0; i < info->len; i++) {
                    msg += (char)data[i];
                }
            } else {
                char buff[3];
                for (size_t i = 0; i < info->len; i++) {
                    sprintf(buff, "%02x ", (uint8_t)data[i]);
                    msg += buff;
                }
            }
            Serial.println(msg);
            if (info->opcode == WS_TEXT)
                client->text("{}");
            else
                client->binary("{}");
        } else {
            if (info->index == 0) {
                if (info->num == 0)
                    Serial.printf("ws[%s][%u] %s\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
                Serial.printf("ws[%s][%u] [%u] [%llu]\n", server->url(), client->id(), info->num, info->len);
            }
            Serial.printf("ws[%s][%u] [%u] %s [%llu - %llu]\n: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);
            if (info->opcode == WS_TEXT) {
                for (size_t i = 0; i < len; i++) {
                    msg += (char)data[i];
                }
            } else {
                char buff[3];
                for (size_t i = 0; i < len; i++) {
                    sprintf(buff, "%02x ", (uint8_t)data[i]);
                    msg += buff;
                }
            }
            Serial.println(msg);
            if ((info->index + len) == info->len) {
                Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
                if (info->final) {
                    Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
                    if (info->message_opcode == WS_TEXT)
                        client->text("text");
                    else
                        client->binary("binary");
                }
            }
        }
    }
}

void initMDNS() {
    MDNS.addService("http", "tcp", 80);
    MDNS.enableArduino(8266);
}

void initWS() {
    // ws.onEvent(onWsEvent);
    // server.addHandler(&ws);
    // events.onConnect([](AsyncEventSourceClient *client) {
    //     client->send("", NULL, millis(), 1000);
    // });
    // server.addHandler(&events);
}

void initOTA() {
    ArduinoOTA.setHostname(config.network()->getHostname());
    ArduinoOTA.begin();

    // ArduinoOTA.onStart([]() {
    //     events.send("Update Start", "ota");
    // });

    // ArduinoOTA.onEnd([]() {
    //     events.send("Update End", "ota");
    // });

    // ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    //     char p[32];
    //     sprintf(p, "Progress: %u%%\n", (progress / (total / 100)));
    //     events.send(p, "ota");
    // });

    // ArduinoOTA.onError([](ota_error_t error) {
    //     if (error == OTA_AUTH_ERROR)
    //         events.send("Auth Failed", "ota");
    //     else if (error == OTA_BEGIN_ERROR)
    //         events.send("Begin Failed", "ota");
    //     else if (error == OTA_CONNECT_ERROR)
    //         events.send("Connect Failed", "ota");
    //     else if (error == OTA_RECEIVE_ERROR)
    //         events.send("Recieve Failed", "ota");
    //     else if (error == OTA_END_ERROR)
    //         events.send("End Failed", "ota");
    // });
}

}  // namespace HttpServer