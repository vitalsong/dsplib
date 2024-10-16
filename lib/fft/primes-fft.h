#pragma once

#include "dsplib/czt.h"
#include "dsplib/fft.h"
#include "dsplib/math.h"
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
        DSPLIB_ASSERT(n_ >= 3, "`n` must be greater than or equal to 3");
        if (n > MAX_DFT_SIZE) {
            const cmplx_t w = expj(-2 * pi / n);
            czt_ = std::make_shared<CztPlan>(n, n, w);
        } else {
            w_ = expj(-2 * pi * arange(n) / n).to_vec();
        }
    }

    [[nodiscard]] arr_cmplx solve(const arr_cmplx& x) const final {
        arr_cmplx y(x.size());
        _dft(x.data(), y.data(), y.size());
        return y;
    }

    [[nodiscard]] int size() const noexcept final {
        return n_;
    }

    void solve(const cmplx_t* restrict x, cmplx_t* restrict y, int n) const final {
        DSPLIB_ASSERT(x != y, "Pointers must be restricted");
        _dft(x, y, n);
    }

private:
    static void _dft_n3(const cmplx_t* restrict x, cmplx_t* restrict y) noexcept {
        constexpr real_t c = -0.5;
        constexpr real_t d = 0.866025403784439;

        y[0].re = x[0].re + x[1].re + x[2].re;
        y[0].im = x[0].im + x[1].im + x[2].im;

        const real_t re1_c = x[1].re * c;
        const real_t im1_d = x[1].im * d;
        const real_t re2_c = x[2].re * c;
        const real_t im2_d = x[2].im * d;
        y[1].re = x[0].re + (re1_c + im1_d) + (re2_c - im2_d);
        y[2].re = x[0].re + (re1_c - im1_d) + (re2_c + im2_d);

        const real_t re1_d = x[1].re * d;
        const real_t im1_c = x[1].im * c;
        const real_t re2_d = x[2].re * d;
        const real_t im2_c = x[2].im * c;
        y[1].im = x[0].im + (-re1_d + im1_c) + (re2_d + im2_c);
        y[2].im = x[0].im + (re1_d + im1_c) + (-re2_d + im2_c);
    }

    //TODO: add dft5, dft7

    static void _dft_slow(const cmplx_t* restrict x, cmplx_t* restrict y, int n, const cmplx_t* restrict tw) noexcept {
        DSPLIB_ASSUME(n <= MAX_DFT_SIZE);
        std::memset(reinterpret_cast<real_t*>(y), 0, n * sizeof(cmplx_t));

        for (uint32_t i = 0; i < n; ++i) {
            y[0] += x[i];
        }

        for (uint32_t k = 1; k < n; ++k) {
            uint32_t iw = 0;
            for (uint32_t i = 0; i < n; ++i) {
                y[k] += x[i] * tw[iw];
                iw += k;
                iw = (iw < n) ? iw : (iw - n);
            }
        }
    }

    void _dft(const cmplx_t* restrict x, cmplx_t* restrict y, int n) const {
        assert(n == n_);

        if (n == 3) {
            _dft_n3(x, y);
            return;
        }

        if (n <= MAX_DFT_SIZE) {
            assert(!w_.empty());
            _dft_slow(x, y, n, w_.data());
            return;
        }

        if (n > MAX_DFT_SIZE) {
            //TODO: noexcept?
            //TODO: call without copy
            assert(czt_ && czt_->size() == n);
            const auto r = czt_->solve(arr_cmplx(x, n));
            std::memcpy(y, r.data(), n * sizeof(cmplx_t));
        }
    }

    const int n_;
    std::shared_ptr<CztPlan> czt_;
    std::vector<cmplx_t> w_;
};

//--------------------------------------------------------------------------------
//TODO: real optimization
class PrimesFftR : public BaseFftPlanR
{
public:
    explicit PrimesFftR(int n)
      : plan_{n} {
    }

    [[nodiscard]] arr_cmplx solve(const arr_real& x) const final {
        return plan_.solve(complex(x));
    }

    [[nodiscard]] int size() const noexcept final {
        return plan_.size();
    }

private:
    PrimesFftC plan_;
};

}   // namespace dsplib