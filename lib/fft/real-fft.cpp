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
    arr_cmplx r(n_);
    this->solve(x, r);
    return r;
}

void RealFftPlan::solve(span_t<real_t> x, mut_span_t<cmplx_t> r) const {
    using namespace std::complex_literals;
    DSPLIB_ASSERT(x.size() == n_, "Input size must be equal FFT size");
    //TODO: n/2+1 size support
    DSPLIB_ASSERT(r.size() == n_, "Output size must be equal FFT size");
    const int n2 = n_ / 2;

    arr_cmplx z(span(reinterpret_cast<const cmplx_t*>(x.data()), n2));
    const auto Z = fft_->solve(z * 0.5);

    {
        const auto Xe = Z[0] + conj(Z[0]);
        const auto Xo = (conj(Z[0]) - Z[0]) * w_[0];
        r[0].re = Xe.re - Xo.im;
        r[0].im = Xe.im + Xo.re;
    }

    for (int i = 1; i < n2; ++i) {
        const auto Zc = conj(Z[n2 - i]);
        const auto Xe = Z[i] + Zc;
        const auto Xo = (Zc - Z[i]) * w_[i];
        r[i].re = Xe.re - Xo.im;
        r[i].im = Xe.im + Xo.re;
        r[n_ - i].re = r[i].re;
        r[n_ - i].im = -r[i].im;
    }

    {
        const auto Xe = Z[0] + conj(Z[0]);
        const auto Xo = conj(Z[0]) - Z[0];
        r[n2] = Xe.re + Xo.im;
    }
}

int RealFftPlan::size() const noexcept {
    return n_;
}

}   // namespace dsplib