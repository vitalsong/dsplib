#include <dsplib/agc.h>
#include <dsplib/math.h>
#include <cmath>

namespace dsplib {

//------------------------------------------------------------------------------------------
struct AgcImpl
{
    int rms_period{100};   ///< rms calculation period
    std::vector<real_t> delay;
    int delay_idx{0};
    real_t trise{0.01};        ///< step size for gain updates (rise)
    real_t tfall{0.01};        ///< step size for gain updates (fall)
    real_t max_gain{4.6052};   ///< max gain coeff
    real_t target{0};          ///< target level (db)
    real_t accum{0};           ///< rms accum
    real_t gain{0};            ///< current gain
};

//------------------------------------------------------------------------------------------
template<typename T>
static Agc::Result<T> _process(AgcImpl& agc, const base_array<T>& x) {
    base_array<T> out(x.size());
    arr_real gain(x.size());
    for (int i = 0; i < x.size(); ++i) {
        real_t d = abs2(x[i]);
        agc.accum += d;
        agc.delay[agc.delay_idx] = d;
        agc.delay_idx = (agc.delay_idx + 1) % agc.rms_period;
        agc.accum -= agc.delay[agc.delay_idx];
        if (agc.accum < 0) {
            agc.accum = 0;
        }

        real_t g = std::exp(agc.gain);
        out[i] = x[i] * g;
        gain[i] = g;

        real_t e = agc.target - (std::log(agc.accum / agc.rms_period) + (2 * agc.gain));
        if (e > 1) {
            agc.gain += agc.trise * e;
        } else {
            agc.gain += agc.tfall * e;
        }

        if (agc.gain > agc.max_gain) {
            agc.gain = agc.max_gain;
        }
    }

    return {out, gain};
}

//------------------------------------------------------------------------------------------
Agc::Agc(real_t target_level, real_t max_gain, int average_len, real_t t_rise, real_t t_fall) {
    _d = std::make_shared<AgcImpl>();

    if (average_len == 0) {
        DSPLIB_THROW("average_len must be greater 0");
    }

    _d->rms_period = average_len;
    _d->trise = t_rise;
    _d->tfall = t_fall;
    _d->target = std::log(target_level);
    _d->max_gain = std::log(std::pow(10, max_gain / 20));
    _d->delay.resize(_d->rms_period);
    std::fill(_d->delay.begin(), _d->delay.end(), 0);
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