#include "Objects/Widget.h"

Widget::Widget(const String& name) : ParamFile(name.c_str()){};

Widget::~Widget(){};

const String Widget::getFilename() const {
    String res;
    res = "/widgets/";
    res += _name;
    res += ".json";
    return res;
}
