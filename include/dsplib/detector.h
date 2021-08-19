#pragma once

#include <dsplib/array.h>
#include <memory>

namespace dsplib {

struct detector_impl;

//reference search correlator
class detector
{
public:
    //h - reference signal
    //threshold - triggered level [0:1]
    //release - time after detection (for signals with wide acorr)
    explicit detector(const arr_cmplx& h, real_t threshold = 0.5, int release = 100);
    ~detector();

    struct result
    {
        bool triggered{false};   ///< detector status
        real_t level{0};         ///< trigger level [0-1] and >= threshold
        arr_cmplx delay;         ///< aligned delay [reference + residual]
    };

    //TODO: add bypass mode
    result process(const arr_cmplx& x);

    void reset();

private:
    std::unique_ptr<detector_impl> _d;
};

}   // namespace dsplib
