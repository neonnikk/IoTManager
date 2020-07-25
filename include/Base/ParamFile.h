#pragma once

#include "ParamStore.h"

class ParamFile : public ParamStore {
   public:
    ParamFile(const String& name);
    ~ParamFile();

    bool save();
    bool load();

   protected:
    virtual const String getFilename() const;

   protected:
    String _name;
};