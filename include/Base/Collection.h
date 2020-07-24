#pragma once

#include <Arduino.h>

template <typename T>
class Collection {
   public:
    virtual T* add(const String& name, const String& assign);

    T* at(size_t index) {
        return _list.at(index);
    }

    T* get(const String name) {
        for (auto item : _list) {
            if (name.equals(item->getName())) {
                return item;
            }
        }
        return NULL;
    }

   private:
    T* last() {
        return _list.at(_list.size() - 1);
    }

   private:
    std::vector<T*> _list;
};