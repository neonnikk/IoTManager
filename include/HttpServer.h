#pragma once

#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266WebServer.h>
#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include <LittleFS.h>
#endif
#include <SPIFFSEditor.h>

extern AsyncWebServer server;
extern AsyncWebSocket ws;
extern AsyncEventSource events;

namespace HttpServer {
void init();
void initOta();
void initMDNS();
void initWS();
}  // namespace HttpServer
