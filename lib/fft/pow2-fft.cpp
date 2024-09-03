#include "fft/pow2-fft.h"

#include <dsplib/math.h>
#include <dsplib/throw.h>
#include <dsplib/types.h>

#include <cmath>
#include <cassert>
#include <cstdint>

namespace dsplib {

namespace {

//generate a half of bitrev table
//table is symmetry, table(n/2, n) == table(0, n/2)+1
std::vector<int32_t> _gen_bitrev_table(int n) noexcept {
    DSPLIB_ASSUME(n % 4 == 0);
    std::vector<int32_t> res(n / 2);
    int h = 1;
    const int s = nextpow2(n);
    for (int i = 0; i < (s - 1); ++i, h *= 2) {
        for (int k = 0; k < h; ++k) {
            res[k] = 2 * res[k];
            res[k + h] = res[k] + 1;
        }
    }
    for (int i = 0; i < n / 2; ++i) {
        res[i] *= 2;
    }
    return res;
}

//generate exp(-1i * 2 * pi * t / n) table
std::vector<cmplx_t> _gen_coeffs_table(int n) noexcept {
    DSPLIB_ASSUME(n % 4 == 0);
    const int n4 = n / 4;
    const int n2 = n / 2;
    const int n3 = 3 * n / 4;
    std::vector<cmplx_t> res(n);
    res[0] = {1, 0};
    res[n4] = {0, -1};
    res[n2] = {-1, 0};
    res[n3] = {0, 1};
    //use only first n/4 samples
    for (int i = 1; i < n4; ++i) {
        const auto v = std::cos(2 * pi * i / n);
        res[i].re = v;
        res[n - i].re = v;
        res[n2 + i].re = -v;
        res[n2 - i].re = -v;
        res[n4 + i].im = -v;
        res[n4 - i].im = -v;
        res[n3 + i].im = v;
        res[n3 - i].im = v;
    }
    return res;
}

//bit reverse array permutation
void _bitreverse(const cmplx_t* restrict x, cmplx_t* restrict y, const int32_t* restrict bitrev, int n) noexcept {
    DSPLIB_ASSUME(n % 2 == 0);
    const int n2 = n / 2;
    for (int i = 0; i < n2; ++i) {
        const auto k = bitrev[i];
        y[i] = x[k];
        y[n2 + i] = x[k + 1];
    }
}

}   // namespace

Pow2FftPlan::Pow2FftPlan(int n)
  : n_{n}
  , l_{nextpow2(n_)} {
    DSPLIB_ASSERT(ispow2(n), "FFT size must be power of 2");
    bitrev_ = _gen_bitrev_table(n);
    coeffs_ = _gen_coeffs_table(n);
}

arr_cmplx Pow2FftPlan::solve(const arr_cmplx& x) const {
    const int n = x.size();
    arr_cmplx y(n);
    solve(x.data(), y.data(), n);
    return y;
}

void Pow2FftPlan::solve(const cmplx_t* x, cmplx_t* y, int n) const {
    DSPLIB_ASSERT(x != y, "Pointers must be restricted");
    _fft(x, y, n);
}

int Pow2FftPlan::size() const noexcept {
    return n_;
}

//TODO: add "small" implementations (2, 4, 8)
void Pow2FftPlan::_fft(const cmplx_t* restrict in, cmplx_t* restrict out, int n) const noexcept {
    DSPLIB_ASSUME(n % 2 == 0);
    DSPLIB_ASSUME(n >= 2);
    DSPLIB_ASSUME(n == (1L << l_));

    //reverse sampling
    _bitreverse(in, out, bitrev_.data(), n);

    uint32_t h = 1;       ///< number of butterflies in clusters (and step between elements)
    uint32_t m = n / 2;   ///< number of clusters (and step for the butterfly table)
    uint32_t r = 2;       ///< number of elements (butterflies * 2) in clusters

    const cmplx_t* restrict cf = coeffs_.data();
    for (int i = 0; i < l_; ++i) {
        cmplx_t* px1 = out;
        cmplx_t* px2 = out + h;
        DSPLIB_ASSUME(px1 != px2);
        for (int j = 0; j < m; ++j) {
            for (int k = 0; k < h; ++k) {
                const cmplx_t x1 = px1[k];
                const cmplx_t x2 = px2[k];
                const cmplx_t p = cf[k * m] * x2;
                px2[k].re = x1.re - p.re;
                px2[k].im = x1.im - p.im;
                px1[k].re = x1.re + p.re;
                px1[k].im = x1.im + p.im;
            }
            DSPLIB_ASSUME(r % 2 == 0);
            px1 += r;
            px2 += r;
        }

        //next cascade
        m /= 2;
        h *= 2;
        r *= 2;
    }
}

}   // namespace dsplib