#pragma once

#include <dsplib/array.h>
#include <dsplib/utils.h>
#include <dsplib/math.h>

namespace dsplib {

/**
 * @brief Dynamic range compressor
 * @see https://www.mathworks.com/help/audio/ref/compressor-system-object.html
 * @todo Add makeup gain
 */
class Compressor
{
public:
    /**
     * @brief Construct a new Compressor object
     * 
     * @param sample_rate Input sample rate (Hz)
     * @param threshold the level (dB) above which gain is applied to the input signal.
     * @param ratio the input/output ratio for signals that overshoot the operation threshold (>=1).
     * @param knee_width the transition area in the compression characteristic.
     * @param attack_time the time (sec) it takes the compressor gain to rise from 10% to 90% of its final value when the input goes above the threshold.
     * @param release_time the time (sec) it takes the compressor gain to drop from 90% to 10% of its final value when the input goes below the threshold.
     */
    explicit Compressor(int sample_rate = 44100, real_t threshold = -10.0, int ratio = 5, real_t knee_width = 0,
                        real_t attack_time = 0.01, real_t release_time = 0.2)
      : T_{threshold}
      , R_{ratio}
      , W_{knee_width} {
        wA_ = std::exp(-std::log(9) / (sample_rate * attack_time));
        wR_ = std::exp(-std::log(9) / (sample_rate * release_time));
        DSPLIB_ASSERT(threshold >= -50 && threshold <= 0, "`threshold` must be in range [-50:0] db");
        DSPLIB_ASSERT(ratio >= 1 && ratio <= 50, "`ratio` must be in range [1:50]");
        DSPLIB_ASSERT(knee_width >= 0 && knee_width <= 20, "`knee_width` must be in range [0:20] db");
        DSPLIB_ASSERT(attack_time >= 0 && attack_time <= 4, "`attack_time` must be in range [0:4] sec");
        DSPLIB_ASSERT(release_time >= 0 && release_time <= 4, "`knee_wrelease_timeidth` must be in range [0:4] sec");
    }

    /**
     * @brief Compressor processing result
     */
    struct Result
    {
        explicit Result(int n)
          : out(n)
          , gain(n) {
        }

        arr_real out;    ///< processed signal, in * gain
        arr_real gain;   ///< applied gain
    };

    /**
     * @brief Process audio frame
     * 
     * @param x [in] Input audio frame
     * @return Result [out] Output pair processed and gain
     */
    Result process(span_real x) {
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

    /**
     * @brief Process audio frame, obj(x) syntax
     * @see process
     */
    Result operator()(span_real x) {
        return this->process(x);
    }

private:
    [[nodiscard]] real_t _compute_gain(const real_t& x) const noexcept {
        //TODO: fast log10?
        const auto xdb = mag2db(dsplib::abs(x) + eps());
        auto xsc = xdb;
        if (xdb >= (T_ + W_ / 2)) {
            xsc = T_ + (xdb - T_) / R_;
        } else if ((xdb > (T_ - W_ / 2)) && (xdb < (T_ + W_ / 2))) {
            xsc = xdb + (1 / R_ - 1) * abs2(xdb - T_ + W_ / 2) / (2 * W_);
        }
        return (xsc - xdb);
    }

    real_t T_{1};    ///< threshold
    int R_{1};       ///< ratio
    real_t W_{0};    ///< knee width
    real_t wA_{1};   ///< attack time coeff
    real_t wR_{1};   ///< release time coeff
    real_t gs_{0};   ///< smoothed gain
};

}   // namespace dsplib