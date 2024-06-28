#pragma once

#include <dsplib/fft.h>
#include <dsplib/utils.h>
#include <dsplib/math.h>

#include "fft/factory.h"

namespace dsplib {

class RealFftPlan : public BaseFftPlanR
{
public:
    explicit RealFftPlan(int n)
      : n_{n}
      , w_(expj(-2 * pi * arange(n / 2) / n).to_vec()) {
        DSPLIB_ASSERT(n % 2 == 0, "FFT size must be even");
        fft_ = create_fft_plan(n / 2);
    }

    [[nodiscard]] arr_cmplx solve(const arr_real& x) const final {
        using namespace std::complex_literals;
        DSPLIB_ASSERT(x.size() == n_, "Input size must be equal FFT size");
        const int n2 = n_ / 2;

        arr_cmplx z(reinterpret_cast<const cmplx_t*>(x.data()), n2);
        const auto Z = fft_->solve(z * 0.5);

        arr_cmplx res(n_);

        {
            const auto Xe = Z[0] + conj(Z[0]);
            const auto Xo = (conj(Z[0]) - Z[0]) * w_[0];
            res[0].re = Xe.re - Xo.im;
            res[0].im = Xe.im + Xo.re;
        }

        for (int i = 1; i < n2; ++i) {
            const auto Zc = conj(Z[n2 - i]);
            const auto Xe = Z[i] + Zc;
            const auto Xo = (Zc - Z[i]) * w_[i];
            res[i].re = Xe.re - Xo.im;
            res[i].im = Xe.im + Xo.re;
            res[n_ - i].re = res[i].re;
            res[n_ - i].im = -res[i].im;
        }

        {
            const auto Xe = Z[0] + conj(Z[0]);
            const auto Xo = conj(Z[0]) - Z[0];
            res[n2] = Xe.re + Xo.im;
        }

        return res;
    }

    [[nodiscard]] int size() const noexcept final {
        return n_;
    }

private:
    const int n_;
    std::shared_ptr<BaseFftPlanC> fft_;
    const std::vector<cmplx_t> w_;
};

}   // namespace dsplib