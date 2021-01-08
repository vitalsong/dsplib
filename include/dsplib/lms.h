#pragma once

#include <dsplib/array.h>

namespace dsplib {

//LMS adaptive filter
class lms
{
public:
    explicit lms(int len, double step_size)
    {
        _len = len;
        _u = zeros(len);
        _w = zeros(len);
        _m = step_size;
    }

    struct result
    {
        arr_real y;   //output
        arr_real e;   //error
        arr_real w;   //weights
    };

    result process(const arr_real& x, const arr_real& d)
    {
        int nx = x.size();
        auto y = zeros(nx);
        auto e = zeros(nx);
        auto tu = concatenate(_u, x);

        for (size_t k = 0; k < nx; k++) {
            _u = tu.slice(k + 1, k + _len + 1);
            y[k] = sum(_w * _u);
            e[k] = d[k] - y[k];
            _w += _m * e[k] * _u;
        }

        return {y, e, _w};
    }

private:
    arr_real _u;
    arr_real _w;
    double _m;
    int _len;
};

}   // namespace dsplib
