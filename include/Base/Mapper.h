#pragma once

#include <Arduino.h>

struct Range {
    int min;
    int max;
    Range(int min, int max) : min{min}, max{max} {};
};

struct MapParams {
    Range in;
    Range out;
    MapParams() : MapParams{Range{0, 0}, Range{0, 0}} {};
    MapParams(Range in, Range out) : in{in}, out{out} {};
};

class Mapper {
   public:
    Mapper(MapParams p) : _params{p} {};

    Mapper(int in_min, int in_max, int out_min, int out_max) : _constrain{true},
                                                               _params{Range{in_min, in_max}, Range{out_min, out_max}} {};

    int mapValue(int value) {
        value = _constrain ? constrain(value, _params.in.min, _params.in.max) : value;
        return map(value, _params.in.min, _params.in.max, _params.out.min, _params.out.max);
    }

    MapParams& getParams() {
        return _params;
    }

   private:
    bool _constrain;
    MapParams _params;
};
