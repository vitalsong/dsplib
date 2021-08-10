#include <dsplib/agc.h>
#include <cmath>

namespace dsplib {

//------------------------------------------------------------------------------------------
struct agc_impl
{
    int rms_period{100};   ///< rms calculation period
    std::vector<double> delay;
    int delay_idx{0};
    double trise{0.01};        ///< step size for gain updates (rise)
    double tfall{0.01};        ///< step size for gain updates (fall)
    double max_gain{4.6052};   ///< max gain coeff
    double target{0};          ///< target level (db)
    double accum{0};           ///< rms accum
    double gain{0};            ///< current gain
};

//------------------------------------------------------------------------------------------
static inline real_t _power(const real_t& v)
{
    return v * v;
}

//------------------------------------------------------------------------------------------
static inline real_t _power(const cmplx_t& v)
{
    return (v.re * v.re) + (v.im * v.im);
}

//------------------------------------------------------------------------------------------
template<typename T>
static agc::result<T> _process(agc_impl& agc, const base_array<T>& x)
{
    base_array<T> out(x.size());
    arr_real gain(x.size());
    for (int i = 0; i < x.size(); ++i) {
        double d = _power(x[i]);
        agc.accum += d;
        agc.delay[agc.delay_idx] = d;
        agc.delay_idx = (agc.delay_idx + 1) % agc.rms_period;
        agc.accum -= agc.delay[agc.delay_idx];
        if (agc.accum < 0) {
            agc.accum = 0;
        }

        float g = expf(agc.gain);
        out[i] = x[i] * g;
        gain[i] = g;

        double e = agc.target - (logf(agc.accum / agc.rms_period) + (2 * agc.gain));
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
agc::agc(double target_level, double max_gain, int average_len, double t_rise, double t_fall)
{
    _d = std::make_unique<agc_impl>();

    if (average_len == 0) {
        throw std::runtime_error("average_len must be greater 0");
    }

    _d->rms_period = average_len;
    _d->trise = t_rise;
    _d->tfall = t_fall;
    _d->target = log(target_level);
    _d->max_gain = log(pow(10, max_gain / 20));
    _d->delay.resize(_d->rms_period);
    std::fill(_d->delay.begin(), _d->delay.end(), 0);
}

//------------------------------------------------------------------------------------------
agc::~agc() = default;

//------------------------------------------------------------------------------------------
agc::result<real_t> agc::process(const arr_real& x)
{
    return _process(*_d, x);
}

//------------------------------------------------------------------------------------------
agc::result<cmplx_t> agc::process(const arr_cmplx& x)
{
    return _process(*_d, x);
}

}   // namespace dsplib