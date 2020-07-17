#include "NetworkManager.h"

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

void init() {
    WiFi.hostname(config.network()->getHostname());
    WiFiMode_t mode = (WiFiMode_t)config.network()->getMode();

    staGotIpEvent = WiFi.onStationModeGotIP([](WiFiEventStationModeGotIP event) {
        pm.info("Connected: " + String(WiFi.status() == WL_CONNECTED ? "yes" : "no"));
        pm.info("http://" + event.ip.toString());
        _connected = true;
    });
    staDisconnectedEvent = WiFi.onStationModeDisconnected([](WiFiEventStationModeDisconnected event) {
        pm.error("Disconnected: " + event.ssid);
        pm.error("Reason: " + String(event.reason, DEC));
        _connected = false;
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

void check_network_status() {
    LedStatus_t _led = LED_OFF;
    if (isNetworkActive()) {
        _lastStatus = Connection::OK;
        if (config.mqtt()->isEnabled()) {
            if (!MqttClient::isConnected()) {
                _led = LED_SLOW;
                MqttClient::connect();
            }
        }
        if (!_intitialized) {
            pm.info("HttpServer");
            HttpServer::init();
            pm.info("WebAdmin");
            web_init();
            perform_updates_check();
            _intitialized = true;
        }
    } else {
        if (_lastStatus == Connection::OK) {
            pm.error("connection lost");
            _led = LED_FAST;
        }
        _lastStatus = Connection::ERROR;
    }
    setLedStatus(_led);
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
    // int connRes;
    // do {
    //     connRes = WiFi.waitForConnectResult();
    //     switch (connRes) {
    //         case -1: {
    //             pm.error("on timeout");
    //         } break;
    //         case WL_NO_SSID_AVAIL: {
    //             pm.error("no network");
    //             keepConnecting = false;
    //         } break;
    //         case WL_CONNECTED: {
    //             keepConnecting = false;
    //         } break;
    //         case WL_CONNECT_FAILED: {
    //             pm.error("check credentials");
    //             keepConnecting = false;
    //         } break;
    //         default:
    //             break;
    //     }

    ts.add(
        NETWORK_CONNECTION, ONE_SECOND_ms, [&](void*) {
            check_network_status();
        },
        nullptr, false);
};

bool startAPMode() {
    setLedStatus(LED_ON);
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