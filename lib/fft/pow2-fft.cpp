#include "fft/pow2-fft.h"

#include <dsplib/math.h>
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

//generate modified exp(-1i * 2 * pi * t / n) table
//{0, 0, n/2, 0, n/4, n/2, 3*n/4, 0, n/8, n/4 ...}
std::vector<cmplx_t> _gen_coeffs_table(int n) noexcept {
    DSPLIB_ASSUME(n % 4 == 0);
    const int n4 = n / 4;
    const int n2 = n / 2;
    std::vector<cmplx_t> tb(n / 2);

    //calculate only first n/4 samples
    tb[0] = {1, 0};
    tb[n4] = {0, -1};
    for (int i = 1; i < n4; ++i) {
        const auto v = std::cos(2 * pi * i / n);
        tb[i].re = v;
        tb[n2 - i].re = -v;
        tb[n4 + i].im = -v;
        tb[n4 - i].im = -v;
    }

    //ensure consistent access to coefficients
    //{0}, {0, n/2}, {0, n/4, 2*n/4, 3*n/4} ...
    //todo: remove first {1, 0} element
    uint32_t h = 1;
    uint32_t m = n / 2;
    uint32_t r = 0;
    const uint32_t s = std::log2(n);
    std::vector<cmplx_t> res(n);
    for (uint32_t i = 0; i < s; ++i) {
        for (int k = 0; k < h; ++k) {
            res[r + k] = tb[k * m];
        }
        r += h;
        h *= 2;
        m /= 2;
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

void Pow2FftPlan::_fft(const cmplx_t* restrict in, cmplx_t* restrict out, int n) const noexcept {
    DSPLIB_ASSUME(n % 2 == 0);
    DSPLIB_ASSUME(n >= 4);
    DSPLIB_ASSUME(n == (1L << l_));

    //reverse sampling
    _bitreverse(in, out, bitrev_.data(), n);

    //i=0
    {
        const uint32_t h = 1;
        const uint32_t r = 2;
        const uint32_t m = n / 2;
        for (int j = 0; j < m; ++j) {
            cmplx_t* px1 = out + (j * r);
            cmplx_t* px2 = px1 + h;
            const cmplx_t x1 = px1[0];
            const cmplx_t w = px2[0];
            px1[0].re = x1.re + w.re;
            px1[0].im = x1.im + w.im;
            px2[0].re = x1.re - w.re;
            px2[0].im = x1.im - w.im;
        }
    }

    //i=1
    {
        const uint32_t h = 2;
        const uint32_t r = 4;
        const uint32_t m = n / 4;
        for (int j = 0; j < m; ++j) {
            cmplx_t* px1 = out + (j * r);
            cmplx_t* px2 = px1 + h;
            //k=0, c={1,0}
            {
                const cmplx_t x1 = px1[0];
                const cmplx_t w = px2[0];
                px1[0].re = x1.re + w.re;
                px1[0].im = x1.im + w.im;
                px2[0].re = x1.re - w.re;
                px2[0].im = x1.im - w.im;
            }
            //k=1, c={0,-1}
            {
                const cmplx_t x1 = px1[1];
                const cmplx_t w = {px2[1].im, -px2[1].re};
                px1[1].re = x1.re + w.re;
                px1[1].im = x1.im + w.im;
                px2[1].re = x1.re - w.re;
                px2[1].im = x1.im - w.im;
            }
        }
    }

    for (int i = 2; i < l_; ++i) {
        const uint32_t h = uint32_t(1) << i;
        const uint32_t r = uint32_t(1) << (i + 1);
        const uint32_t m = n >> (i + 1);
        const cmplx_t* pw = coeffs_.data() + h - 1;
        for (int j = 0; j < m; ++j) {
            cmplx_t* px1 = out + (j * r);
            cmplx_t* px2 = px1 + h;

            //k=0, c={1,0}
            {
                const cmplx_t x1 = px1[0];
                const cmplx_t w = px2[0];
                px1[0].re = x1.re + w.re;
                px1[0].im = x1.im + w.im;
                px2[0].re = x1.re - w.re;
                px2[0].im = x1.im - w.im;
            }

            //k=[1:h]
            for (int k = 1; k < h; ++k) {
                const cmplx_t x1 = px1[k];
                const cmplx_t w = pw[k] * px2[k];
                px1[k].re = x1.re + w.re;
                px1[k].im = x1.im + w.im;
                px2[k].re = x1.re - w.re;
                px2[k].im = x1.im - w.im;
            }
        }
    }
}

}   // namespace dsplib