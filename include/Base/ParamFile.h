#pragma once

#include "ParamStore.h"

class ParamFile : public ParamStore {
   public:
    ParamFile(const char* name);
    ~ParamFile();

    bool save();
    bool load();

   protected:
    virtual const String getFilename() const;

   protected:
    char* _name;
};