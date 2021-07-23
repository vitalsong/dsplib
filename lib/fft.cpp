#include <dsplib/fft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

#include "dft-tables.h"

#include <string.h>
#include <assert.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
//bit reverse array permutation
static void _reversal(cmplx_t* w, int n)
{
    auto tb = tables::bitrev_table(n);
    const int32_t* bitrev = tb->data();
    for (int i = 0; i < n; ++i) {
        int k = bitrev[i];
        std::swap(w[i], w[k]);
    }
}

//-------------------------------------------------------------------------------------------------
static inline void _btrf(cmplx_t& x1, cmplx_t& x2, cmplx_t w)
{
    w *= x2;
    x2 = x1 - w;
    x1 = x1 + w;
}

//-------------------------------------------------------------------------------------------------
static void _fft(cmplx_t* x, const cmplx_t* w, int n)
{
    assert(n % 2 == 0);

    //reverse sampling
    _reversal(x, n);

    int h = 1;       ///< number of butterflies in clusters (and step between elements)
    int m = n / 2;   ///< number of clusters (and step for the butterfly table)
    int r = 2;       ///< number of elements (butterflies * 2) in clusters

    cmplx_t* px;                 ///< pointer to go through the signal vector
    const cmplx_t* pw;           ///< pointer to go through the coefficient vector
    const int L = nextpow2(n);   ///< number of DFT runs

    //cascades
    for (int i = 0; i < L; ++i) {
        px = x;
        //clusters
        for (int j = 0; j < m; ++j) {
            pw = w;
            //butterflies
            for (int k = 0; k < h; ++k) {
                _btrf(px[k], px[k + h], *pw);
                pw += m;
            }
            px += r;
        }

        //next cascade
        m /= 2;
        h *= 2;
        r *= 2;
    }
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fft(const arr_cmplx& arr)
{
    const int n = 1L << nextpow2(arr.size());
    arr_cmplx r = zeros(n);
    memcpy(r.data(), arr.data(), arr.size() * sizeof(cmplx_t));
    auto w = tables::dft_table(n);
    _fft(r.data(), w->data(), n);
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fft(const arr_real& arr)
{
    const int n = 1L << nextpow2(arr.size());
    arr_cmplx r = arr_cmplx(zeros(n));

    for (int i = 0; i < arr.size(); ++i) {
        r[i].re = arr[i];
        r[i].im = 0;
    }

    auto w = tables::dft_table(n);
    _fft(r.data(), w->data(), n);
    return r;
}

}   // namespace dsplib
