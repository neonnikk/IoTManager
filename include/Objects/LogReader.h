#pragma once

#include <Arduino.h>
#include <functional>

#ifdef ESP32
#include "LITTLEFS.h"
#else
#include "LittleFS.h"
#endif

#include "Objects/LogMetadata.h"

typedef std::function<void(LogMetadata *, uint8_t *)> LogEntryHandler;

class LogReader {
   private:
    uint8_t buf[sizeof(Entry)];
    File _file;
    LogMetadata *_meta;
    unsigned long _start;
    LogEntryHandler _handler;
    size_t _pos;
    bool _active;

   public:
    LogReader(LogMetadata *meta, unsigned long start, LogEntryHandler handler) : _meta{meta},
                                                                                 _start{start},
                                                                                 _handler{handler},
                                                                                 _pos{0},
                                                                                 _active{false} {
    }

    void setActive(bool value) {
        String filename = _meta->getStartTime() < _start ? _meta->getDataFile() : _meta->getDataFile(_start);
        _file = LittleFS.open(filename.c_str(), FILE_READ);

        if (_file) {
            _active = true;
        }
    }

    bool isActive() {
        return _active;
    }

    void loop() {
        if (!_active || (!_file)) {
            return;
        }

        if (_file.available()) {
            _file.readBytes((char *)&buf, sizeof(Entry));
            _handler(_meta, buf);
            _pos++;
        } else {
            _file.close();
            _active = false;
        }
    }
};