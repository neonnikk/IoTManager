#include "Objects/Widget.h"

Widget::Widget(const char* name) : ParamFile(name){};

Widget::~Widget(){};

const String Widget::getFilename() const {
    String res;
    res = "/widgets/";
    res += _name;
    res += ".json";
    return res;
}
