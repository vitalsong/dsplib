#include <dsplib/fft.h>
#include <dsplib/math.h>

#include "dft-tables.h"

#include <string.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
static void _dft(const cmplx_t* x, cmplx_t* y, const cmplx_t* w, int nx)
{
    int k;
    for (int m = 0; m < nx; ++m)
    {
        k = 0;
        y[m] = 0;
        for (int n=0; n < nx; ++n)
        {
            y[m] += x[n] * w[k];
            k = (k + m) % nx;
        }
    }
}

//-------------------------------------------------------------------------------------------------
arr_cmplx dft(const arr_cmplx &x)
{
    int n = x.size();
    arr_cmplx y(n);
    const cmplx_t* w = tables::dft_table(n);
    _dft(x.data(), y.data(), w, n);
    return y;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx dft(const arr_real &x)
{
    int n = x.size();
    arr_cmplx y(n);
    arr_cmplx xx(n);
    for (int i=0; i < n; ++i)
    {
        xx[i].re = x[i];
        xx[i].im = 0;
    }

    const cmplx_t* w = tables::dft_table(n);
    _dft(xx.data(), y.data(), w, n);
    return y;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx idft(const arr_cmplx &x)
{
    int n = x.size();
    arr_cmplx y(n);
    auto xx = conj(x);
    const cmplx_t* w = tables::dft_table(n);
    _dft(xx.data(), y.data(), w, n);
    y = conj(y) / n;
    return y;
}

}   ///< dsplib
