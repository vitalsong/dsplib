#pragma once

#include <dsplib/array.h>
#include <memory>

namespace dsplib {

struct AgcImpl;

//Adaptively adjust gain for constant signal level output
class Agc
{
public:
    //target_level - target output power level
    //max_gain - maximum power gain
    //average_len - length of averaging window
    //t_rise - step size for gain updates (rise)
    //t_fall - step size for gain updates (fall)
    explicit Agc(real_t target_level = 1, real_t max_gain = 60.0, int average_len = 100, real_t t_rise = 0.01,
                 real_t t_fall = 0.01);

    template<typename T>
    struct Result
    {
        base_array<T> y;
        arr_real gain;
    };

    Result<real_t> process(const arr_real& x);
    Result<cmplx_t> process(const arr_cmplx& x);

    template<typename T>
    Result<T> operator()(const base_array<T>& x) {
        return this->process(x);
    }

private:
    std::shared_ptr<AgcImpl> _d;
};

}   // namespace dsplib
