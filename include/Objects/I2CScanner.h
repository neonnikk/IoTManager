#pragma once

#include "Base/BusScanner.h"

class I2CScanner : public BusScanner {
   public:
    I2CScanner();

   protected:
    bool onInit() override;
    boolean onScan() override;

   private:
    uint8_t _addr;
};