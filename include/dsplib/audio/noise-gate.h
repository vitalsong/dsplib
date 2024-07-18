#pragma once

#include <dsplib/math.h>
#include <dsplib/array.h>

namespace dsplib {

//Dynamic range gate
class NoiseGate
{
public:
    explicit NoiseGate(int sample_rate = 44100, real_t threshold = -10.0, real_t attack_time = 0.05,
                       real_t release_time = 0.02, real_t hold_time = 0.05)
      : tlin_{db2mag(threshold)}
      , wA_{std::exp(-std::log(real_t(9)) / (sample_rate * attack_time))}
      , wR_{std::exp(-std::log(real_t(9)) / (sample_rate * release_time))}
      , tH_{int(std::floor(hold_time * sample_rate))} {
        DSPLIB_ASSERT(threshold >= -140 && threshold <= 0, "`threshold` must be in range [-140:0] db");
        DSPLIB_ASSERT(attack_time >= 0 && attack_time <= 4, "`attack_time` must be in range [0:4] sec");
        DSPLIB_ASSERT(release_time >= 0 && release_time <= 4, "`release_time` must be in range [0:4] sec");
        DSPLIB_ASSERT(hold_time >= 0 && hold_time <= 4, "`hold_time` must be in range [0:4] sec");
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

    const real_t tlin_;   ///< threshold (linear domain)
    const real_t wA_;     ///< attack time coefficient
    const real_t wR_;     ///< release time coefficient
    const int tH_;        ///< hold time (samples)
    int cA_{0};           ///< hold counter for attack
    real_t lg_{0};        ///< prev gain
};

}   // namespace dsplib