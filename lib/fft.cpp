#include <dsplib/fft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

#include "dft-tables.h"

#include <string.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
static inline int _get_bit(int a, int pos) {
    return (a >> pos) & 0x1;
}

//-------------------------------------------------------------------------------------------------
static inline void _set_bit(int &a, int pos, int bit)
{
    a &= ~(1 << pos);
    a |= (bit << pos);
}

//-------------------------------------------------------------------------------------------------
//bit reverse for s bits
static inline int _bitrev(int a, int s)
{
    int r = 0;
    for (int i=0; i < ((s+1)/2); ++i)
    {
        _set_bit(r, (s-i-1), _get_bit(a, i));
        _set_bit(r, i, _get_bit(a, (s-i-1)));
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
//bit reverse array permutation
static void _reversal(cmplx_t* w, int n)
{
    int k;
    const int s = nextpow2(n);
    for (int i=0; i < n; ++i)
    {
        k = _bitrev(i, s);
        if (k > i) {
            std::swap(w[i], w[k]);
        }
    }
}

//-------------------------------------------------------------------------------------------------
static inline void _btrf(cmplx_t& x1, cmplx_t& x2, const cmplx_t& w)
{
    cmplx_t t0 = x2 * w;
    x2 = x1 - t0;
    x1 = x1 + t0;
}

//-------------------------------------------------------------------------------------------------
//FFT calculation
static void _fft(cmplx_t* x, const cmplx_t* w, int n)
{
    //reverse sampling
    _reversal(x, n);

    //butterfly in stages
    int h = 1;                          ///< number of butterflies in clusters (and step between elements)
    int m = n/2;                        ///< number of clusters (and step for the butterfly table)
    int r = 2;                          ///< number of elements (butterflies * 2) in clusters
    cmplx_t* px;                        ///< pointer to go through the signal vector
    const cmplx_t* pw;                  ///< pointer to go through the coefficient vector
    const int L = nextpow2(n);          ///< number of DFT runs

    //passage through cascades
    for (int i=0; i < L; ++i)
    {
        px = x;
        //passage through clusters
        for (int j=0; j < m; ++j)
        {
            pw = w;
            //passage through butterflies
            for (int k=0; k < h; ++k)
            {
                _btrf(px[k], px[k+h], *pw);
                pw += m;
            }

            px += r;
        }

        //update the parameters for calculating the next cascade
        m /= 2;
        h *= 2;
        r *= 2;
    }
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fft(const arr_cmplx &arr)
{
    int n = 1L << nextpow2(arr.size());
    arr_cmplx r = zeros(n - arr.size());
    r = concatenate(arr, r);
    const cmplx_t* w = tables::dft_table(n);
    _fft(r.data(), w, n);
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fft(const arr_real &arr)
{
    int n = 1L << nextpow2(arr.size());
    arr_cmplx r = arr_cmplx(zeros(n));

    for (int i=0; i < arr.size(); ++i)
    {
        r[i].re = arr[i];
        r[i].im = 0;
    }

    const cmplx_t* w = tables::dft_table(n);
    _fft(r.data(), w, n);
    return r;
}

}   ///< dsplib
