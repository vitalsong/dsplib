#include <dsplib/interp.h>
#include <dsplib/utils.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
interp_filter::interp_filter(const arr_real &h, int m) : _m(m), _fir(h)
{
    //nothing to do
}

//-------------------------------------------------------------------------------------------------
arr_real interp_filter::process(const arr_real &x)
{
    arr_real y = zeros(x.size() * _m);
    for (int i=0; i < x.size(); ++i) {
        y[i * _m] = x[i] * _m;
    }

    return _fir.process(y);
}

} ///< dsplib
