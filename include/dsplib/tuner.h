#pragma once

#include <dsplib/array.h>

#include <cmath>

namespace dsplib {

//up/down freq converter
//out = in * exp(1i * 2 * pi * freq * t)
class Tuner
{
public:
    //sample_rate - sample rate of signal (Hz)
    //freq - tune freq in range (-sample_rate/2 : sample_rate/2) (Hz)
    explicit Tuner(int sample_rate, real_t freq)
      : _fs{sample_rate}
      , _freq{freq} {
        DSPLIB_ASSERT(std::abs(_freq) < (_fs / 2), "Tuner freq must be in range (-sample_rate/2 : sample_rate/2)");
    }

    arr_cmplx process(const arr_cmplx& x) {
        const int n = x.size();
        arr_cmplx r(n);
        for (int i = 0; i < n; i++) {
            const real_t phase = 2 * pi * _freq * _phase / _fs;
            const cmplx_t w = {std::cos(phase), std::sin(phase)};
            r[i] = x[i] * w;
            ++_phase;
            _phase = (_phase < _fs) ? _phase : 0;
        }
        return r;
    }

    [[nodiscard]] real_t freq() const noexcept {
        return _freq;
    }

    [[nodiscard]] int sample_rate() const noexcept {
        return _fs;
    }

    arr_cmplx operator()(const arr_cmplx& x) {
        return this->process(x);
    }

private:
    int _fs;
    real_t _freq;
    int _phase{0};
};

}   // namespace dsplib
