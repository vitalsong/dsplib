#pragma once

#include <dsplib/math.h>
#include <dsplib/utils.h>

#include "dft-tables.h"

#include <cassert>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
//bit reverse array permutation
static void _reversal(cmplx_t* restrict w, const int32_t* restrict bitrev, int n) {
    for (int i = 0; i < n; ++i) {
        const int k = bitrev[i];
        std::swap(w[i], w[k]);
    }
}

//-------------------------------------------------------------------------------------------------
static inline void _btrf(cmplx_t& x1, cmplx_t& x2, cmplx_t w) {
    w *= x2;
    x2 = x1 - w;
    x1 = x1 + w;
}

//-------------------------------------------------------------------------------------------------
static void _fft2(cmplx_t* restrict x, const cmplx_t* restrict w, const int32_t* restrict bitrev, int n) {
    assert(n % 2 == 0);

    //reverse sampling
    _reversal(x, bitrev, n);

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
class fft2_plan
{
public:
    explicit fft2_plan(int n) {
        const int n2 = 1L << nextpow2(n);
        assert(n2 == n);
        _n = n;
        _birev = tables::bitrev_table(n);
        _dft_tb = tables::dft_table(n);
    }

    arr_cmplx solve(const arr_cmplx& x) const {
        arr_cmplx r = x;
        _fft2(r.data(), _dft_tb->data(), _birev->data(), _n);
        return r;
    }

private:
    tables::bitrev_ptr _birev;
    tables::dft_ptr _dft_tb;
    int _n;
};

}   // namespace dsplib
