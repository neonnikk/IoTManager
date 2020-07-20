#pragma once

#include "Base/ParamFile.h"

class Runtime : public ParamFile {
   public:
    Runtime();

    void publish();

   protected:
    void onAdd(KeyValue* item) override;
    void onUpdate(KeyValue* item) override;

   private:
    void fireEvent(KeyValue* item);
    void publishMqtt(KeyValue* item);
};

extern Runtime runtime;