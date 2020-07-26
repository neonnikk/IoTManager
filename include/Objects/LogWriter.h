#pragma once

#include <Arduino.h>
#include <functional>
#include <LittleFS.h>

#include "Objects/LogMetadata.h"
#include "Objects/LogEntry.h"
#include "Objects/LogBuffer.h"

class LogWriter {
   private:
    File _file;
    LogMetadata *_meta;
    std::list<LogEntry *> *_buffer;
    bool _active;
    size_t _pos;

   public:
    LogWriter(LogMetadata &meta, std::list<LogEntry *> *buffer) : _meta{&meta},
                                                                  _buffer{buffer},
                                                                  _active{false},
                                                                  _pos{0} {
    }

    void setActive(bool value = true) {
        _active = value;
    }

    bool isActive() {
        return _active;
    }

    void loop() {
        if (!_active) {
            return;
        }

        if (!_file) {
            _file = LittleFS.open(_meta->getDataFile().c_str(), FILE_APPEND);
        }

        if (_file) {
            if (_buffer->size()) {
                LogEntry *item = _buffer->front();
                _meta->add(item);
                item->write(_file);
                delete item;
                _buffer->pop_front();
            } else {
                _file.close();
                _meta->store();
                _active = false;
            }
        }
    }
};
