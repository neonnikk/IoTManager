#pragma once

#include <Arduino.h>

#define MEDIAN_FILTER_SIZE (7)
#define AVERAGE_FILTER_SIZE 8
#define STOPPER 0
#define MAX_SENSOR_VALUE 999999
#define MIN_SENSOR_VALUE -999999

class Filter {
   public:
    virtual ~Filter() = default;
    virtual bool process(float) = 0;
    virtual float get() = 0;
};

class Passthrough : public Filter {
   public:
    Passthrough() : Filter(), _value{0}, _min_value(MAX_SENSOR_VALUE), _max_value(MIN_SENSOR_VALUE){};

    bool process(float value) override {
        if (_min_value > value) {
            _min_value = value;
        }
        if (_max_value < value) {
            _max_value = value;
        }
        return true;
    }

    float get() override {
        return _value;
    }

   private:
    float _value, _min_value, _max_value;
};

class MovingAverage : public Filter {
   public:
    MovingAverage(size_t size);
    ~MovingAverage();
    bool process(float) override;
    float get() override;

   private:
    float *_buffer;
    size_t _size;
    size_t _n;
};

class Smoothing : public Filter {
   private:
    float _buffer[4];

   public:
    Smoothing() {
        memset(&_buffer, 0, sizeof(_buffer[0]) * 4);
    };
    bool process(float) override;
    float get() override;
};

class MedianFilter : public Filter {
   public:
    MedianFilter() : successor{NULL}, scan{NULL}, scanold{NULL}, median{NULL} {
        memset(&_buffer, 0, sizeof(pair) * MEDIAN_FILTER_SIZE);
    };
    bool process(float) override;
    float get() override;

   private:
    struct pair {
        struct pair *point; /* Pointers forming list linked in sorted order */
        uint16_t value;     /* Values to sort */
    };
    struct pair _buffer[MEDIAN_FILTER_SIZE]; /* Buffer of nwidth pairs */
    struct pair *datpoint = _buffer;         /* Pointer into circular buffer of data */
    struct pair tail = {NULL, STOPPER};      /* Chain stopper */
    struct pair head = {&tail, 0};           /* Pointer to head (largest) of linked list.*/
    struct pair *successor;                  /* Pointer to successor of replaced data item */
    struct pair *scan;                       /* Pointer used to scan down the sorted list */
    struct pair *scanold;                    /* Previous value of scan */
    struct pair *median;                     /* Pointer to median */
};