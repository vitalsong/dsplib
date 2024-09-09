#pragma once

#include "dsplib/array.h"
#include "dsplib/types.h"
#include <dsplib/fft.h>
#include <dsplib/assert.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------------------
//FFT implementation for small sizes
class SmallFftPow2C : public BaseFftPlanC
{
public:
    friend class SmallFftPow2R;

    explicit SmallFftPow2C(int n)
      : n_{n} {
        DSPLIB_ASSERT((n >= 1) && (n <= 8), "only small power-of-two sizes are supported: 1, 2, 4, 8");
    }

    ~SmallFftPow2C() override {
    }

    void solve(const cmplx_t* x, cmplx_t* y, int n) const final {
        DSPLIB_ASSERT(n == n_, "input size error");
        switch (n_) {
        case 1:
            y[0] = x[0];
            break;
        case 2:
            _fft_n2(x, y);
            break;
        case 4:
            _fft_n4(x, y);
            break;
        case 8:
            _fft_n8(x, y);
            break;
        default:
            DSPLIB_THROW("size not supported");
            break;
        }
    }

    [[nodiscard]] arr_cmplx solve(const arr_cmplx& x) const final {
        arr_cmplx y(x.size());
        solve(x.data(), y.data(), x.size());
        return y;
    }

    [[nodiscard]] int size() const noexcept final {
        return n_;
    }

private:
    static void _fft_n2(const cmplx_t* restrict x, cmplx_t* restrict y) noexcept {
        y[0].re = x[0].re + x[1].re;
        y[0].im = x[0].im + x[1].im;
        y[1].re = x[0].re - x[1].re;
        y[1].im = x[0].im - x[1].im;
    }

    static void _fft_n4(const cmplx_t* restrict x, cmplx_t* restrict y) noexcept {
        y[0].re = x[0].re + x[1].re + x[2].re + x[3].re;
        y[0].im = x[0].im + x[1].im + x[2].im + x[3].im;
        y[1].re = x[0].re + x[1].im - x[2].re - x[3].im;
        y[1].im = x[0].im - x[1].re - x[2].im + x[3].re;
        y[2].re = x[0].re - x[1].re + x[2].re - x[3].re;
        y[2].im = x[0].im - x[1].im + x[2].im - x[3].im;
        y[3].re = x[0].re - x[1].im - x[2].re + x[3].im;
        y[3].im = x[0].im + x[1].re - x[2].im - x[3].re;
    }

    static void _fft_n8(const cmplx_t* restrict x, cmplx_t* restrict y) noexcept {
        cmplx_t p1[4];
        cmplx_t p2[4];
        cmplx_t r1[4];
        cmplx_t r2[4];

        p1[0] = x[0] + x[4];
        p1[1] = x[1] + x[5];
        p1[2] = x[2] + x[6];
        p1[3] = x[3] + x[7];
        _fft_n4(p1, r1);

        p2[0] = x[0] - x[4];
        p2[1] = (x[1] - x[5]) * cmplx_t{0.707106781186548, -0.707106781186548};
        p2[2].re = x[2].im - x[6].im;
        p2[2].im = x[6].re - x[2].re;
        p2[3] = (x[3] - x[7]) * cmplx_t{-0.707106781186548, -0.707106781186548};
        _fft_n4(p2, r2);

        for (int i = 0; i < 4; ++i) {
            *y++ = r1[i];
            *y++ = r2[i];
        }
    }

    const int n_;
};

//-------------------------------------------------------------------------------------------------------------
class SmallFftPow2R : public BaseFftPlanR
{
public:
    explicit SmallFftPow2R(int n)
      : n_{n} {
        DSPLIB_ASSERT((n >= 1) && (n <= 8), "only small power-of-two sizes are supported: 1, 2, 4, 8");
    }

    ~SmallFftPow2R() override {
    }

    void solve(const real_t* x, cmplx_t* y, int n) const final {
        DSPLIB_ASSERT(n == n_, "input size error");
        switch (n_) {
        case 1:
            y[0] = x[0];
            break;
        case 2:
            _fft_n2(x, y);
            break;
        case 4:
            _fft_n4(x, y);
            break;
        case 8:
            _fft_n8(x, y);
            break;
        default:
            DSPLIB_THROW("size not supported");
            break;
        }
    }

    [[nodiscard]] arr_cmplx solve(const arr_real& x) const final {
        arr_cmplx y(x.size());
        solve(x.data(), y.data(), x.size());
        return y;
    }

    [[nodiscard]] int size() const noexcept final {
        return n_;
    }

private:
    static void _fft_n2(const real_t* restrict x, cmplx_t* restrict y) noexcept {
        y[0].re = x[0] + x[1];
        y[0].im = 0;
        y[1].re = x[0] - x[1];
        y[1].im = 0;
    }

    static void _fft_n4(const real_t* restrict x, cmplx_t* restrict y) noexcept {
        y[0].re = x[0] + x[1] + x[2] + x[3];
        y[0].im = 0;
        y[1].re = x[0] - x[2];
        y[1].im = -x[1] + x[3];
        y[2].re = x[0] - x[1] + x[2] - x[3];
        y[2].im = 0;
        y[3].re = x[0] - x[2];
        y[3].im = x[1] - x[3];
    }

    static void _fft_n8(const real_t* restrict x, cmplx_t* restrict y) noexcept {
        real_t p1[4];
        cmplx_t r1[4];
        p1[0] = x[0] + x[4];
        p1[1] = x[1] + x[5];
        p1[2] = x[2] + x[6];
        p1[3] = x[3] + x[7];
        SmallFftPow2R::_fft_n4(p1, r1);

        cmplx_t p2[4];
        cmplx_t r2[4];
        p2[0] = x[0] - x[4];
        p2[1] = (x[1] - x[5]) * cmplx_t{0.707106781186548, -0.707106781186548};
        p2[2].re = 0;
        p2[2].im = x[6] - x[2];
        p2[3] = (x[3] - x[7]) * cmplx_t{-0.707106781186548, -0.707106781186548};
        SmallFftPow2C::_fft_n4(p2, r2);

        for (int i = 0; i < 4; ++i) {
            *y++ = r1[i];
            *y++ = r2[i];
        }
    }

    const int n_;
};

}   // namespace dsplib