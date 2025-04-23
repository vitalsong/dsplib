#include <dsplib/fft.h>
#include <dsplib/utils.h>
#include <dsplib/math.h>

#include "fft/real-fft.h"

namespace dsplib {

RealFftPlan::RealFftPlan(int n)
  : n_{n}
  , fft_{fft_plan_c(n / 2)}
  , w_(expj(-2 * pi * arange(n / 2) / n).to_vec()) {   //TODO: optimization (like in real ifft module)
    DSPLIB_ASSERT(n % 2 == 0, "FFT size must be even");
}

arr_cmplx RealFftPlan::solve(span_t<real_t> x) const {
    using namespace std::complex_literals;
    DSPLIB_ASSERT(x.size() == n_, "Input size must be equal FFT size");
    const int n2 = n_ / 2;

    arr_cmplx z(span(reinterpret_cast<const cmplx_t*>(x.data()), n2));
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

int RealFftPlan::size() const noexcept {
    return n_;
}

}   // namespace dsplib