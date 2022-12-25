#pragma once

#include <dsplib/array.h>
#include <memory>

namespace dsplib {

struct DetectorImpl;

//reference search correlator
class Detector
{
public:
    //h - reference signal
    //threshold - triggered level [0:1]
    //release - time after detection (for signals with wide acorr)
    explicit Detector(const arr_cmplx& h, real_t threshold = 0.5, int release = 100);

    struct Result
    {
        bool triggered{false};   ///< detector status
        real_t level{0};         ///< trigger level [0-1] and >= threshold
        arr_cmplx delay;         ///< aligned delay [reference + residual]
    };

    //TODO: add bypass mode
    Result process(const arr_cmplx& x);

    Result operator()(const arr_cmplx& x) {
        return this->process(x);
    }

    void reset();

private:
    std::shared_ptr<DetectorImpl> _d;
};

using detector [[deprecated]] = Detector;

}   // namespace dsplib
