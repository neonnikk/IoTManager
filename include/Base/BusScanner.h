#pragma once

#include <Arduino.h>

class BusScanner {
   public:
    BusScanner(const char* tag) : _resStr{""},
                                  _intialized{false} {
        _tag = strdup(tag);
    }

    virtual ~BusScanner() {
        free(_tag);
    };

    const String results() {
        return _resStr;
    }

    const String tag() {
        return _tag;
    }

    bool scan() {
        if (!_intialized) {
            bool res = onInit();
            if (!res) {
                _resStr = String(F("ошибка инициализации"));
                return true;
            }
            _intialized = true;
        }
        return onScan();
    }

   protected:
    virtual bool onInit() {
        return true;
    };

    virtual bool onScan() {
        return true;
    };

   protected:
    String _resStr;

   private:
    char* _tag;
    bool _intialized;
};
