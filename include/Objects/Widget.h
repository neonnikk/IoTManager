#pragma once

#include "Base/ParamFile.h"

class Widget : public ParamFile {
   public:
    Widget(const char* name);
    ~Widget();

   protected:
    const String getFilename() const override;
};
