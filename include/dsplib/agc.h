#pragma once

#include <dsplib/array.h>

namespace dsplib {

//Adaptively adjust gain for constant signal level output
class agc
{
public:
    //target_level - target output power level
    //max_gain - maximum power gain
    //average_len - length of averaging window
    //t_rise - step size for gain updates (rise)
    //t_fall - step size for gain updates (fall)
    explicit agc(double target_level = 1, double max_gain = 60.0, int average_len = 100, double t_rise = 0.01,
                 double t_fall = 0.01);

    struct result
    {
        arr_real y;
        arr_real gain;
    };

    result process(const arr_real& x);

private:
    void _update_detector();

    double _max_gain;      ///< max gain (db)
    double _trise;         ///< step size for gain updates (rise)
    double _tfall;         ///< step size for gain updates (fall)
    double _zvalue;        ///< exp filter delay
    double _gain_m;        ///< gain coeff
    double _max_gain_m;    ///< max gain coeff
    double _min_gain_m;    ///< min gain coeff
    double _level;         ///< target level (db)
    double _rms_acum;      ///< rms accum
    double _rms_counter;   ///< rms calculation counter
    double _rms_period;    ///< rms calculation period
};

}   // namespace dsplib
