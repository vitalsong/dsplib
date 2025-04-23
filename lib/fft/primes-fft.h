#pragma once

#include "dsplib/czt.h"
#include "dsplib/fft.h"
#include "dsplib/math.h"
#include "dsplib/span.h"
#include "dsplib/utils.h"

#include <cstring>

namespace dsplib {

//boundary for calculating DFT instead of CZT
constexpr int MAX_DFT_SIZE = 41;

//--------------------------------------------------------------------------------
class PrimesFftC : public FftPlanC
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

    [[nodiscard]] arr_cmplx solve(span_t<cmplx_t> x) const final {
        arr_cmplx r(n_);
        this->solve(x, r);
        return r;
    }

    void solve(span_t<cmplx_t> x, mut_span_t<cmplx_t> r) const final {
        DSPLIB_ASSERT(x.size() == n_, "array size error");
        DSPLIB_ASSERT(r.size() == n_, "array size error");
        //TODO: check x and r overlaped
        _dft(x.data(), r.data(), n_);
    }

    [[nodiscard]] int size() const noexcept final {
        return n_;
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
        DSPLIB_ASSUME(n == n_);

        if (n <= MAX_DFT_SIZE) {
            assert(!w_.empty());
            _dft_slow(x, y, n, w_.data());
            return;
        }

        if (n > MAX_DFT_SIZE) {
            //TODO: noexcept?
            assert(czt_ && czt_->size() == n);
            czt_->solve(span(x, n), span(y, n));
        }
    }

    const int n_;
    std::shared_ptr<CztPlan> czt_;
    std::vector<cmplx_t> w_;
};

//--------------------------------------------------------------------------------
//TODO: real optimization
class PrimesFftR : public FftPlanR
{
public:
    explicit PrimesFftR(int n)
      : plan_{n} {
    }

    [[nodiscard]] arr_cmplx solve(span_t<real_t> x) const final {
        return plan_.solve(complex(x));
    }

    [[nodiscard]] int size() const noexcept final {
        return plan_.size();
    }

private:
    PrimesFftC plan_;
};

}   // namespace dsplib