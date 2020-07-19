#pragma once

#include "Base/KeyValueStore.h"

class Runtime;

extern Runtime runtime;

class Runtime : public KeyValueFile {
   public:
    Runtime(const char* filename) : KeyValueFile(filename){};

    void publish();

   protected:
    void onAdd(KeyValue* item) override;
    void onUpdate(KeyValue* item) override;

   private:
    void fireEvent(KeyValue* item);
    void publishMqtt(KeyValue* item);
};
