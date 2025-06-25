#include <dsplib/agc.h>
#include <dsplib/math.h>
#include <dsplib/assert.h>
#include <dsplib/ma-filter.h>
#include <cmath>

namespace dsplib {

//------------------------------------------------------------------------------------------
struct AgcImpl
{
    real_t trise{0.01};        ///< step size for gain updates (rise)
    real_t tfall{0.01};        ///< step size for gain updates (fall)
    real_t max_gain{4.6052};   ///< max gain coeff
    real_t target{0};          ///< target level (db)
    real_t gain{1.0};          ///< current gain
    MAFilterR maflt{100};      ///< moving average filter (power)
};

//------------------------------------------------------------------------------------------
template<typename T>
static Agc::Result<T> _process(AgcImpl& agc, const base_array<T>& x) {
    static const auto e = dsplib::eps();
    const int nx = x.size();
    base_array<T> out(nx);
    arr_real gain(nx);
    for (int i = 0; i < nx; ++i) {
        const auto input_power = agc.maflt(abs2(x[i])) + e;
        DSPLIB_ASSUME(input_power > 0);
        const real_t err = agc.target - (std::log(input_power) + (2 * agc.gain));
        if (err > 1) {
            agc.gain += agc.trise * err;
        } else {
            agc.gain += agc.tfall * err;
        }

        if (agc.gain > agc.max_gain) {
            agc.gain = agc.max_gain;
        }

        gain[i] = std::exp(agc.gain);
        out[i] = x[i] * gain[i];
    }

    return {out, gain};
}

//------------------------------------------------------------------------------------------
Agc::Agc(real_t target_level, real_t max_gain, int average_len, real_t t_rise, real_t t_fall) {
    _d = std::make_shared<AgcImpl>();
    DSPLIB_ASSERT(average_len > 0, "average_len must be greater 0");
    _d->trise = t_rise;
    _d->tfall = t_fall;
    _d->target = std::log(target_level);
    _d->max_gain = std::log(std::pow(10, max_gain / 20));
    _d->maflt = MAFilterR(average_len);
}

//------------------------------------------------------------------------------------------
Agc::Result<real_t> Agc::process(const arr_real& x) {
    return _process(*_d, x);
}

//------------------------------------------------------------------------------------------
Agc::Result<cmplx_t> Agc::process(const arr_cmplx& x) {
    return _process(*_d, x);
}

}   // namespace dsplib