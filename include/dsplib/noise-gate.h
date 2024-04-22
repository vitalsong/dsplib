#pragma once

#include "dsplib/math.h"
#include "dsplib/array.h"
#include "dsplib/utils.h"

namespace dsplib {

class NoiseGate
{
public:
    explicit NoiseGate(int sample_rate = 44100, real_t threshold = -10.0, real_t attack_time = 0.05,
                       real_t release_time = 0.02, real_t hold_time = 0.05)
      : tlin_{db2mag(threshold)} {
        wA_ = std::exp(-std::log(9) / (sample_rate * attack_time));
        wR_ = std::exp(-std::log(9) / (sample_rate * release_time));
        tA_ = std::floor(attack_time * sample_rate);
        tR_ = std::floor(release_time * sample_rate);
        tH_ = std::floor(hold_time * sample_rate);
    }

    struct Result
    {
        explicit Result(int n)
          : out(n)
          , gain(n) {
        }
        arr_real out;
        arr_real gain;
    };

    Result process(const arr_real& x) {
        const int n = x.size();
        Result res(n);
        for (int i = 0; i < n; ++i) {
            const auto gc = (dsplib::abs(x[i]) >= tlin_) ? 1 : 0;
            lg_ = _smooth_gain(gc);
            res.gain[i] = lg_;
            res.out[i] = x[i] * res.gain[i];
        }
        return res;
    }

    Result operator()(const arr_real& x) {
        return this->process(x);
    }

private:
    real_t _smooth_gain(real_t gc) noexcept {
        if (gc == lg_) {
            return gc;
        }

        //attack
        if (gc < lg_) {
            if (cA_ < tH_) {
                cA_ += 1;
                return lg_;
            } else {
                return (wA_ * lg_) + ((1 - wA_) * gc);
            }
        } else {
            //release
            cA_ = 0;
            return (wR_ * lg_) + ((1 - wR_) * gc);
        }

        return lg_;
    }

    real_t tlin_;
    real_t wA_;
    real_t wR_;
    int tA_;
    int tR_;
    int tH_;
    int cA_{0};
    real_t lg_{0};
};

}   // namespace dsplib