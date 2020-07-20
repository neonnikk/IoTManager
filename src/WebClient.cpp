#include "WebClient.h"

#ifdef ESP8266
#include <ESP8266HTTPClient.h>
#else
#include <HTTPClient.h>
#endif

namespace WebClient {

const String get(const String& url) {
    String res = "";
    WiFiClient client;
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
