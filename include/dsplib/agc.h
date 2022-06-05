#pragma once

#include <dsplib/array.h>
#include <memory>

namespace dsplib {

struct agc_impl;

//Adaptively adjust gain for constant signal level output
class agc
{
public:
    //target_level - target output power level
    //max_gain - maximum power gain
    //average_len - length of averaging window
    //t_rise - step size for gain updates (rise)
    //t_fall - step size for gain updates (fall)
    explicit agc(double target_level = 1, double max_gain = 60.0, int average_len = 100, double t_rise = 0.01,
                 double t_fall = 0.01);

    ~agc();

    template<typename T>
    struct result
    {
        base_array<T> y;
        arr_real gain;
    };

    result<real_t> process(const arr_real& x);
    result<cmplx_t> process(const arr_cmplx& x);

    template<typename T>
    result<T> operator()(const base_array<T>& x) {
        return this->process(x);
    }

private:
    std::unique_ptr<agc_impl> _d;
};

}   // namespace dsplib
