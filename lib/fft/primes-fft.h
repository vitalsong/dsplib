#pragma once

#include "dsplib/czt.h"
#include "dsplib/fft.h"
#include "dsplib/math.h"
#include "dsplib/throw.h"
#include "dsplib/utils.h"

#include <cassert>
#include <cstring>

namespace dsplib {

//boundary for calculating DFT instead of CZT
constexpr int MAX_DFT_SIZE = 41;

//--------------------------------------------------------------------------------
class PrimesFftC : public BaseFftPlanC
{
public:
    explicit PrimesFftC(int n)
      : n_{n} {
        DSPLIB_ASSERT(isprime(n_), "`n` must be a prime number");
        if (n > MAX_DFT_SIZE) {
            const cmplx_t w = expj(-2 * pi / n);
            czt_ = std::make_shared<CztPlan>(n, n, w);
        } else if (n <= MAX_DFT_SIZE) {
            w_ = expj(-2 * pi * arange(n) / n).to_vec();
            return;
        }
    }

    [[nodiscard]] arr_cmplx solve(const arr_cmplx& x) const final {
        arr_cmplx y(x);
        _dft(y.data(), y.size());
        return y;
    }

    [[nodiscard]] int size() const noexcept final {
        return n_;
    }

    void solve(const cmplx_t* x, cmplx_t* y, int n) const final {
        if (x != y) {
            std::memcpy(y, x, n * sizeof(cmplx_t));
        }
        _dft(y, n);
    }

private:
    static void _dft_n2(cmplx_t* x) noexcept {
        cmplx_t y0, y1;

        y0.re = x[0].re + x[1].re;
        y0.im = x[0].im + x[1].im;

        y1.re = (x[0].re - x[1].re);
        y1.im = (x[0].im - x[1].im);

        x[0] = y0;
        x[1] = y1;
    }

    static void _dft_n3(cmplx_t* x) noexcept {
        cmplx_t y0, y1, y2;
        constexpr real_t c = -0.5;
        constexpr real_t d = 0.866025403784439;

        y0.re = x[0].re + x[1].re + x[2].re;
        y0.im = x[0].im + x[1].im + x[2].im;

        const real_t re1_c = x[1].re * c;
        const real_t im1_d = x[1].im * d;
        const real_t re2_c = x[2].re * c;
        const real_t im2_d = x[2].im * d;
        y1.re = x[0].re + (re1_c + im1_d) + (re2_c - im2_d);
        y2.re = x[0].re + (re1_c - im1_d) + (re2_c + im2_d);

        const real_t re1_d = x[1].re * d;
        const real_t im1_c = x[1].im * c;
        const real_t re2_d = x[2].re * d;
        const real_t im2_c = x[2].im * c;
        y1.im = x[0].im + (-re1_d + im1_c) + (re2_d + im2_c);
        y2.im = x[0].im + (re1_d + im1_c) + (-re2_d + im2_c);

        x[0] = y0;
        x[1] = y1;
        x[2] = y2;
    }

    static void _dft_slow(cmplx_t* x, int n, const cmplx_t* tw) noexcept {
        cmplx_t y[MAX_DFT_SIZE];
        std::memset(&y[0].re, 0, sizeof(y));

        for (int i = 0; i < n; ++i) {
            y[0] += x[i];
        }

        for (int k = 1; k < n; ++k) {
            int iw = 0;
            for (int i = 0; i < n; ++i) {
                y[k] += x[i] * tw[iw];
                iw += k;
                iw = (iw < n) ? iw : (iw - n);
            }
        }

        std::memcpy(x, y, n * sizeof(cmplx_t));
    }

    void _dft(cmplx_t* x, int n) const {
        using namespace std::complex_literals;

        assert(n == n_);

        if (n == 2) {
            _dft_n2(x);
            return;
        }

        if (n == 3) {
            _dft_n3(x);
            return;
        }

        if (n <= MAX_DFT_SIZE) {
            assert(!w_.empty());
            _dft_slow(x, n, w_.data());
            return;
        }

        //TODO: noexcept?
        //TODO: call without copy
        assert(czt_ && czt_->size() == n);
        const auto y = czt_->solve(arr_cmplx(x, n));
        std::memcpy(x, y.data(), n * sizeof(cmplx_t));
    }

    const int n_;
    std::shared_ptr<CztPlan> czt_;
    std::vector<cmplx_t> w_;
};

//--------------------------------------------------------------------------------
class PrimesFftR : public BaseFftPlanR
{
public:
    explicit PrimesFftR(int n)
      : plan_{n} {
    }

    [[nodiscard]] arr_cmplx solve(const arr_real& x) const final {
        return plan_.solve(arr_cmplx(x));
    }

    [[nodiscard]] int size() const noexcept final {
        return plan_.size();
    }

private:
    PrimesFftC plan_;
};

}   // namespace dsplib