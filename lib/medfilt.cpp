#include <dsplib/medfilt.h>
#include <dsplib/utils.h>
#include <cassert>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
//insert new value in sorted list
static void _update_sort(real_t* x, int nx, real_t v_new, real_t v_old) {
    int pos;

    //search old value
    pos = 0;
    while ((x[pos] != v_old) && (pos < nx - 1)) {
        ++pos;
    }

    //erase old value
    if (pos != (nx - 1)) {
        std::memmove(x + pos, x + pos + 1, (nx - pos - 1) * sizeof(real_t));
    }

    //search pos for new value
    pos = 0;
    while ((x[pos] < v_new) && (pos < nx - 1)) {
        ++pos;
    }

    //prepare buffer for insert
    if (pos != (nx - 1)) {
        std::memmove(x + pos + 1, x + pos, (nx - pos - 1) * sizeof(real_t));
    }

    //insert new value
    x[pos] = v_new;
}

//------------------------------------------------------------------------------------------
MedianFilter::MedianFilter(int n) {
    assert(n >= 3);
    _i = 0;
    _n = (n / 2) * 2 + 1;
    _d = zeros(_n);
    _s = zeros(_n);
}

//------------------------------------------------------------------------------------------
arr_real MedianFilter::process(const arr_real& x) {
    auto y = zeros(x.size());
    for (int i = 0; i < x.size(); ++i) {
        _i = (_i + 1) % _n;
        _update_sort(_s.data(), _n, x[i], _d[_i]);
        _d[_i] = x[i];
        y[i] = _s[_n / 2];
    }

    return y;
}

//------------------------------------------------------------------------------------------
arr_real medfilt(arr_real& x, int n) {
    auto flt = MedianFilter(n);
    auto y = flt.process(x);
    return y;
}

}   // namespace dsplib
