#pragma once

#include <Arduino.h>

class Metric {
   private:
    unsigned long long _loop_cnt;
    unsigned long long _total_mu;
    unsigned long _start;
    unsigned long _lps;

   public:
    Metric();
    void start();
    void finish();
    unsigned long getLps();
    void reset();
};

extern Metric metric;