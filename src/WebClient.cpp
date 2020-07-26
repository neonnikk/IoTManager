#include "WebClient.h"

#ifdef ESP8266
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#else
#include <HTTPClient.h>
#endif

namespace WebClient {

const String get(const String& url) {
    String res = "";
#ifdef ESP8266
    BearSSL::WiFiClientSecure client;
    client.setInsecure();
#else
    WiFiClient client;
#endif
    HTTPClient http;
    http.begin(client, url);
    if (http.GET() == HTTP_CODE_OK) {
        res = http.getString();
    } else {
        res = "error";
    }
    http.end();
    return res;
}

}  // namespace WebClient
