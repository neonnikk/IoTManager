#pragma once

#include <Base/Writer.h>
#include <Objects/LogReader.h>
#include <Objects/LogWriter.h>
#include <Objects/LogEntry.h>
#include <Objects/LogMetadata.h>
#include <Objects/LogBuffer.h>

class LoggerTask {
   public:
    LoggerTask(size_t id, const String& name, unsigned long interval, unsigned long period);
    ~LoggerTask();

    void readEntries(LogEntryHandler h);
    void update();
    void clear();

    LogMetadata* getMetadata();

   public:
    const String asJson();
    const String asCVS();

   private:
    void publishFile();

   private:
    LogMetadata _meta;
    LogBuffer _buffer;
    size_t _id;
    unsigned long _interval;
    unsigned long _period;
    size_t _limit;
    unsigned long _lastUpdated;
    unsigned long _bufferFlushed;
    LogReader* _reader;
    LogWriter* _writer;
};