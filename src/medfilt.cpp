#include <dsplib/medfilt.h>
#include <string.h>
#include <assert.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
//insert new value in sorted list
static void _update_sort(real_t* x, int nx, real_t v_new, real_t v_old)
{
    int pos;

    //search old value
    pos = 0;
    while ((x[pos] != v_old) && (pos < nx-1)) {
        ++pos;
    }

    //erase old value
    if (pos != (nx-1)) {
        memmove(x+pos, x+pos+1, (nx-pos-1) * sizeof(real_t));
    }

    //search pos for new value
    pos = 0;
    while ((x[pos] < v_new) && (pos < nx-1)) {
        ++pos;
    }

    //prepare buffer for insert
    if (pos != (nx-1)) {
        memmove(x+pos+1, x+pos, (nx-pos-1) * sizeof(real_t));
    }

    //insert new value
    x[pos] = v_new;
}

//------------------------------------------------------------------------------------------
median_filter::median_filter(int n)
{
    assert(n >= 3);
    _i = 0;
    _n = (n / 2) * 2 + 1;
    _d = arr_real::zeros(_n);
    _s = arr_real::zeros(_n);
}

//------------------------------------------------------------------------------------------
arr_real median_filter::filter(const arr_real &x)
{
    auto y = arr_real::zeros(x.size());
    for (int i=0; i < x.size(); ++i)
    {
        _i = (_i + 1) % _n;
        _update_sort(_s.data(), _n, x[i], _d[_i]);
        _d[_i] = x[i];
        y[i] = _s[_n / 2];
    }

    return y;
}

//------------------------------------------------------------------------------------------
arr_real medfilt(arr_real &x, int n)
{
    auto flt = median_filter(n);
    auto y = flt.filter(x);
    return y;
}

}   ///< dsplib
