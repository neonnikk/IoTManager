#include "Base/ParamFile.h"

#include "LittleFS.h"

#define FILE_READ "r"
#define FILE_WRITE "w"
#define FILE_APPEND "a"

ParamFile::ParamFile(const char* name) : ParamStore() {
    _name = strdup(name);
}

ParamFile::~ParamFile() {
    free(_name);
}

const String ParamFile::getFilename() const {
    return _name;
}

bool ParamFile::save() {
    bool res = false;
    auto file = LittleFS.open(getFilename().c_str(), FILE_WRITE);
    if (file) {
        DynamicJsonBuffer buf;
        JsonObject& obj = buf.createObject();
        KeyValueStore::save(obj);
        obj.printTo(file);
        res = true;
    }
    file.close();
    return res;
}

bool ParamFile::load() {
    bool res = false;
    auto file = LittleFS.open(getFilename().c_str(), FILE_READ);
    if (file) {
        if (file.available()) {
            DynamicJsonBuffer buf;
            JsonObject& obj = buf.parse(file);
            KeyValueStore::load(obj);
            res = true;
        }
        file.close();
    };
    return res;
}