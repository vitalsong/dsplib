#include <dsplib/agc.h>
#include <cmath>

namespace dsplib {

//------------------------------------------------------------------------------------------
agc::agc(double target_level, double max_gain, int average_len, double t_rise, double t_fall)
{
    if (average_len == 0) {
        throw std::runtime_error("average_len must be greater 0");
    }

    _max_gain = max_gain;
    _rms_period = average_len;
    _trise = t_rise;
    _tfall = t_fall;
    _zvalue = 0;
    _rms_counter = 0;
    _rms_acum = 0;
    _gain_m = 1;
    _level = 2 * log(target_level);
    _max_gain_m = pow(10, (max_gain / 20));
    _min_gain_m = pow(10, (-max_gain / 20));
}

//------------------------------------------------------------------------------------------
void agc::_update_detector()
{
    //calculate output of detector
    double rms = (_gain_m * _gain_m) * _rms_acum;
    _rms_counter = 0;
    _rms_acum = 0;

    //residual
    double z = _level - log(rms);

    if (z > 0) {
        _zvalue = _zvalue + (z * _trise);
    } else {
        _zvalue = _zvalue + (z * _tfall);
    }

    //natural antilog (and root entered as 0.5)
    double gain = exp(_zvalue / 2);

    //gain/suppression limiting
    _gain_m = gain;
    if (gain < _min_gain_m) {
        _gain_m = _min_gain_m;
    } else if (gain > _max_gain_m) {
        _gain_m = _max_gain_m;
    }
}

//------------------------------------------------------------------------------------------
agc::result agc::process(const arr_real& x)
{
    arr_real y(x.size());
    arr_real g(x.size());
    for (int i = 0; i < x.size(); ++i) {
        _rms_acum += (x[i] * x[i]) / _rms_period;
        _rms_counter += 1;
        if (_rms_counter == _rms_period) {
            _update_detector();
        }

        //output
        y[i] = x[i] * _gain_m;
        g[i] = _gain_m;
    }

    return {y, g};
}

}   // namespace dsplib