#pragma once

#include <dsplib/fft.h>

namespace dsplib {

//reference signal detector
class Detector
{
public:
    //ref - reference signal
    //threshold - triggered level [0:1]
    explicit Detector(const arr_cmplx& ref, real_t threshold = 0.5);

    struct State
    {
        bool triggered{false};
        real_t level{0};   ///< detected peak level (>=threshold)
        int position{0};   ///< start reference position for out signal
        arr_cmplx out;     ///< delayed input signal
    };

    //TODO: add bypass mode
    State process(const arr_cmplx& sig);

    State operator()(const arr_cmplx& sig) {
        return this->process(sig);
    }

    [[nodiscard]] int delay_len() const noexcept {
        return fft_.size() / 2;
    }

private:
    int nh_;
    FftPlan fft_;
    arr_cmplx buf_;
    arr_cmplx ref_;
    real_t threshold_;
    int wpos_{0};
};

}   // namespace dsplib
