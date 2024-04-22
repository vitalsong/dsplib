#pragma once

#include <dsplib/array.h>
#include <dsplib/utils.h>

namespace dsplib {

//Dynamic range limiter
class Limiter
{
public:
    explicit Limiter(int sample_rate = 44100, real_t threshold = -10.0, real_t knee_width = 0, real_t attack_time = 0,
                     real_t release_time = 0.2)
      : T_{threshold}
      , W_{knee_width} {
        wA_ = std::exp(-std::log(9) / (sample_rate * attack_time));
        wR_ = std::exp(-std::log(9) / (sample_rate * release_time));
        DSPLIB_ASSERT(threshold >= -50 && threshold <= 0, "`threshold` must be in range [-50:0] db");
        DSPLIB_ASSERT(knee_width >= 0 && knee_width <= 20, "`knee_width` must be in range [0:20] db");
        DSPLIB_ASSERT(attack_time >= 0 && attack_time <= 4, "`attack_time` must be in range [0:4] sec");
        DSPLIB_ASSERT(release_time >= 0 && release_time <= 4, "`knee_wrelease_timeidth` must be in range [0:4] sec");
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
            const auto gc = _compute_gain(x[i]);
            if (gc <= gs_) {
                gs_ = (wA_ * gs_) + (1 - wA_) * gc;
            } else {
                gs_ = (wR_ * gs_) + (1 - wR_) * gc;
            }
            const auto glin = db2mag(gs_);
            res.gain[i] = glin;
            res.out[i] = x[i] * glin;
        }
        return res;
    }

    Result operator()(const arr_real& x) {
        return this->process(x);
    }

private:
    real_t _compute_gain(real_t x) const noexcept {
        const auto xdb = mag2db(dsplib::abs(x) + eps());
        auto xsc = xdb;
        if (xdb > (T_ + W_ / 2)) {
            xsc = T_;
        } else if ((xdb >= (T_ - W_ / 2)) && (xdb <= (T_ + W_ / 2))) {
            xsc = xdb - abs2(xdb - T_ + W_ / 2) / (2 * W_);
        }
        return (xsc - xdb);
    }

    real_t T_{1};    ///< threshold
    real_t W_{0};    ///< knee width
    real_t wA_{1};   ///< attack time coeff
    real_t wR_{1};   ///< release time coeff
    real_t gs_{0};   ///< smoothed gain
};

}   // namespace dsplib