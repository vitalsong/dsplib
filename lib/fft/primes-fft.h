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
        DSPLIB_ASSERT(n_ >= 5, "`n` must be greater than or equal to 5");
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
    //TODO: add dft5, dft7

    static void _dft_slow(const cmplx_t* restrict x, cmplx_t* restrict y, uint32_t n,
                          const cmplx_t* restrict tw) noexcept {
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