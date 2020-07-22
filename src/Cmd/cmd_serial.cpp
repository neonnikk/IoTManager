#include "Cmd.h"

static const char *MODULE = "cmd_serial";

void cmd_serialEnd() {
    if (mySerial) {
        mySerial->end();
        delete mySerial;
    }
}

void cmd_serialBegin() {
    uint32_t baud = atoi(sCmd.next());
    int8_t rx = atoi(sCmd.next());
    int8_t tx = atoi(sCmd.next());

    cmd_serialEnd();
#ifdef ESP8266
    mySerial = new SoftwareSerial(rx, tx);
    mySerial->begin(baud);
#else
    mySerial = new HardwareSerial(2);
    mySerial->begin(rx, tx);
#endif
    term = new Terminal(mySerial);
    term->setOnReadLine([](const char *str) {
        addCommand(str);
    });
}

void cmd_serialWrite() {
    String payload = sCmd.next();
    if (term) {
        term->println(payload.c_str());
    }
}
