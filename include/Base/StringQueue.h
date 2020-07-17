#pragma once

#include <Arduino.h>

#include <list>

class StringQueue {
   private:
    std::list<String> _pool;

   public:
    void push(const String& item) {
        _pool.push_back(item);
    }

    bool pop(String& item) {
        bool res = available();
        if (res) {
            item = _pool.front();
            _pool.pop_front();
        }
        return res;
    }

    size_t available() {
        return !_pool.empty();
    };
};
