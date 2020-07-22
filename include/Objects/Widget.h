#pragma once

#include "Base/ParamFile.h"

class Widget : public ParamFile {
   public:
    Widget(const String& name);
    ~Widget();

   protected:
    const String getFilename() const override;
};
