#include <dsplib/utils.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
arr_real range(real_t start, real_t stop, real_t step)
{
    int n = ::round((stop - start) / step);
    arr_real r(n);
    real_t v = start;
    for (int i=0; i < n; ++i)
    {
        r[i] = v;
        v += step;
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real range(real_t stop)
{
    return range(0, stop, 1);
}

} ///< dsplib