#include "Metric.h"

#include "Consts.h"

Metric metric;

Metric::Metric() : _loop_cnt{0},
                   _start{0},
                   _lps{0} {};

void Metric::start() {
    _start = micros();
}

void Metric::finish() {
    _total_mu += micros() - _start;
    _loop_cnt++;

    if (_total_mu > ONE_SECOND_mu) {
        _lps = _loop_cnt / (_total_mu / 1000);
        reset();
    }
}

unsigned long Metric::getLps() {
    return _lps;
}

void Metric::reset() {
    _total_mu = 0;
    _loop_cnt = 0;
}