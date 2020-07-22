#include "Objects/LoggerTask.h"

#include "MqttClient.h"
#include "Clock.h"
#include "Runtime.h"
#include "PrintMessage.h"
#include "Utils/FileUtils.h"
#include "Utils/TimeUtils.h"

// static const char* MODULE = "LoggerTask";

LoggerTask::LoggerTask(size_t id, const String& name, unsigned long interval, unsigned long period) : _meta{name},
                                                                                                      _id{id},
                                                                                                      _interval{interval},
                                                                                                      _period{period},
                                                                                                      _limit{10},
                                                                                                      _lastUpdated{0},
                                                                                                      _bufferFlushed{millis()},
                                                                                                      _reader{NULL},
                                                                                                      _writer{NULL} {}

LoggerTask::~LoggerTask() {
    if (_writer) delete _writer;
    if (_reader) delete _reader;
}

void LoggerTask::clear() {
    removeFile(_meta.getDataFile().c_str());
    removeFile(_meta.getMetaFile().c_str());
    _meta.reset();
}

void LoggerTask::update() {
    if (_reader) {
        if (_reader->isActive()) {
            _reader->loop();
        } else {
            delete _reader;
            _reader = NULL;
        }
        return;
    }

    if (_writer) {
        if (_writer->isActive()) {
            _writer->loop();
        } else {
            delete _writer;
            _writer = NULL;
        }
        return;
    }

    if (!_lastUpdated || (millis_since(_lastUpdated) >= _interval)) {
        String objId = _meta.getName();
        String value = runtime.read(objId);
        if (value.isEmpty()) {
            return;
        }
        if (now.hasSynced()) {
            LogEntry entry = LogEntry(now.getEpoch(), value.toFloat());
            _buffer.push(entry);
            MqttClient::publishChart(objId, entry.asChartEntry());
        }
        _lastUpdated = millis();
    }

    if (millis_since(_bufferFlushed) > ONE_MINUTE_ms) {
        if (_buffer.size()) {
            _writer = new LogWriter(_meta, _buffer);
            _writer->setActive();
        }
        _bufferFlushed = millis();
    }
}

const String LoggerTask::asCVS() {
    String res;
    res += String(_id, DEC);
    res += ";";
    res += getMetadata()->getName();
    res += ";";
    res += String(getMetadata()->getCount(), DEC);
    res += ";";
    res += prettyBytes(getMetadata()->getSize());
    res += ";";
    res += getMetadata()->getStartDateTimeStr();
    res += ";";
    res += getMetadata()->getFinishDateTimeStr();
    return res;
}

const String LoggerTask::asJson() {
    String res;
    res += "{\"id\":\"";
    res += String(_id, DEC);
    res += "\",";
    res += "\"name\":\"";
    res += getMetadata()->getName();
    res += "\",";
    res += "\"entries\":\"";
    res += String(getMetadata()->getCount(), DEC);
    res += "\",";
    res += "\"size\":\"";
    res += prettyBytes(getMetadata()->getSize());
    res += "\"";
    res += "}";
    return res;
}

LogMetadata* LoggerTask::getMetadata() {
    return &_meta;
}

void LoggerTask::readEntries(LogEntryHandler h) {
    unsigned long start = now.getEpoch() - (_period / 1000);
    _reader = new LogReader(&_meta, start, h);
    _reader->setActive(true);
}