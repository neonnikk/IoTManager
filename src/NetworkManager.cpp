#include "NetworkManager.h"

#include "StatusLed.h"
#include "HttpServer.h"
#include "PrintMessage.h"
#include "Utils/TimeUtils.h"

namespace NetworkManager {

static const char* MODULE = "Network";

enum class Connection : bool {
    ERROR,
    OK,
};

Connection _lastStatus = Connection::ERROR;
bool _connected = false;
bool _intitialized = false;

static WiFiEventHandler staGotIpEvent, staDisconnectedEvent;

void onConnect(IPAddress ip) {
    _connected = true;

    pm.info("http://" + ip.toString());
    if (!_intitialized) {
        pm.info("HttpServer");
        HttpServer::init();
        pm.info("WebAdmin");
        web_init();
        perform_updates_check();
        _intitialized = true;
    }

    ts.add(
        NETWORK_CONNECTION, ONE_SECOND_ms * 5, [&](void*) {
            if (isNetworkActive()) {
                if (config.mqtt()->isEnabled()) {
                    if (MqttClient::isConnected()) {
                        led.set(LedStatus::OFF);
                        _lastStatus = Connection::OK;
                        return;
                    } else {
                        led.set(LedStatus::FAST);
                    }
                } else {
                    led.set(LedStatus::OFF);
                }
            } else {
                if (_lastStatus == Connection::OK) {
                    pm.error("connection lost");
                    led.set(LedStatus::FAST);
                }
                _lastStatus = Connection::ERROR;
            }
        },
        nullptr, false);
}

void onDisconnect(uint8_t reason) {
    led.set(LedStatus::SLOW);
    pm.error("disconnected: " + String(reason, DEC));
    _connected = false;
}

void init() {
    led.set(LedStatus::SLOW);

    WiFi.hostname(config.network()->getHostname());
    WiFiMode_t mode = (WiFiMode_t)config.network()->getMode();

    staGotIpEvent = WiFi.onStationModeGotIP([](WiFiEventStationModeGotIP event) {
        if (WiFi.status() == WL_CONNECTED) {
            onConnect(event.ip);
        }
    });

    staDisconnectedEvent = WiFi.onStationModeDisconnected([](WiFiEventStationModeDisconnected event) {
        onDisconnect(event.reason);
    });

    switch (mode) {
        case WIFI_AP:
            startAPMode();
            break;
        case WIFI_STA:
            startSTAMode();
            break;
        default:
            break;
    }
}

boolean isNetworkActive() {
    return _connected;
}

IPAddress getHostIP() {
    return WiFi.getMode() == WIFI_STA ? WiFi.localIP() : WiFi.localIP();
}

void startSTAMode() {
    ts.remove(WIFI_SCAN);

    String ssid, passwd;
    config.network()->getSSID(WIFI_STA, ssid);
    config.network()->getPasswd(WIFI_STA, passwd);

    pm.info("STA Mode: " + ssid);

    WiFi.mode(WIFI_STA);
    if (ssid.isEmpty() && passwd.isEmpty()) {
        WiFi.begin();
    } else {
        if (WiFi.begin(ssid.c_str(), passwd.c_str()) == WL_CONNECT_FAILED) {
            pm.error("on begin");
        }
    }
    WiFi.setAutoReconnect(true);
};

bool startAPMode() {
    WiFi.setAutoReconnect(false);

    led.set(LedStatus::ON);

    String ssid, passwd;
    config.network()->getSSID(WIFI_AP, ssid);
    config.network()->getPasswd(WIFI_AP, passwd);
    pm.info("AP Mode: " + ssid);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid.c_str(), passwd.c_str());

    String hostIpStr = WiFi.softAPIP().toString();
    pm.info("http://" + hostIpStr);
    runtime.write("ip", hostIpStr.c_str());

    ts.add(
        WIFI_SCAN, 10 * ONE_SECOND_ms,
        [&](void*) {
            if (WiFi.scanComplete()) {
                startScaninng();
            }
        },
        nullptr, true);
    return true;
};

void startScaninng() {
    WiFi.scanNetworksAsync([](int n) {
        if (!n) {
            pm.info(String("no networks found"));
        }
        String ssid;
        config.network()->getSSID(WIFI_STA, ssid);
        pm.info("found: " + String(n, DEC));
        bool res = false;
        for (int8_t i = 0; i < n; i++) {
            if (ssid == WiFi.SSID(i)) {
                res = true;
            }
            pm.info((res ? "*" : "") + String(i, DEC) + ") " + WiFi.SSID(i));
        }
        if (res) startSTAMode();
    },
                           true);
};
}  // namespace NetworkManager