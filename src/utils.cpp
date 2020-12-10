#include <dsplib/utils.h>
#include <algorithm>

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

//-------------------------------------------------------------------------------------------------
template<class T>
T _repelem(const T& x, int n)
{
    if (n == 0) {
        return T(0);
    }

    if (n == 1) {
        return x;
    }

    T r(x.size() * n);
    for (size_t i = 0; i < x.size(); i++) {
        int t1 = i * n;
        int t2 = t1 + n;
        std::fill(r.data() + t1, r.data() + t2, x[i]);
    }
    
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real repelem(const arr_real& x, int n) {
    return _repelem<arr_real>(x, n);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx repelem(const arr_cmplx& x, int n) {
    return _repelem<arr_cmplx>(x, n);
}

//-------------------------------------------------------------------------------------------------
arr_real flip(const arr_real& x) {
    arr_real r(x);
    std::reverse(r.begin(), r.end());
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx flip(const arr_cmplx& x) {
    arr_cmplx r(x);
    std::reverse(r.begin(), r.end());
    return r;
}

} ///< dsplib