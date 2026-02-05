#include "fft/real-ifft.h"

namespace dsplib {

namespace {

std::vector<cmplx_t> _icoeffs4(int n) noexcept {
    DSPLIB_ASSUME(n % 4 == 0);
    const int n4 = n / 4;
    const int n2 = n / 2;
    std::vector<cmplx_t> res(n / 2);
    res[0] = {1, 0};
    res[n4] = {0, 1};
    //use only first n/4 samples
    for (int i = 1; i < n4; ++i) {
        const auto v = std::cos(2 * pi * i / n);
        res[i].re = v;
        res[n2 - i].re = -v;
        res[n4 + i].im = v;
        res[n4 - i].im = v;
    }
    return res;
}

std::vector<cmplx_t> _icoeffs2(int n) noexcept {
    DSPLIB_ASSUME(n % 2 == 0);
    const int n2 = n / 2;
    std::vector<cmplx_t> res(n / 2);
    res[0] = {1, 0};
    //FIXIT: not optimize
    for (int i = 1; i < n2; ++i) {
        res[i].re = std::cos(2 * pi * i / n);
        res[i].im = std::sin(2 * pi * i / n);
    }
    return res;
}

std::vector<cmplx_t> _irfft_coeffs(int n) noexcept {
    DSPLIB_ASSUME(n % 2 == 0);
    if (n % 4 == 0) {
        return _icoeffs4(n);
    }
    return _icoeffs2(n);
}

}   // namespace

RealIfftPlan::RealIfftPlan(int n)
  : n_{n}
  , fft_{fft_plan_c(n / 2)}
  , w_(_irfft_coeffs(n)) {
    DSPLIB_ASSERT(n % 2 == 0, "ifft size must be even");
}

arr_real RealIfftPlan::solve(span_t<cmplx_t> x) const {
    arr_real r(n_);
    this->solve(x, r);
    return r;
}

void RealIfftPlan::solve(span_t<cmplx_t> x, mut_span_t<real_t> r) const {
    assert(n_ % 2 == 0);
    DSPLIB_ASSERT((x.size() == n_) || (x.size() == n_ / 2 + 1), "input size must be n/2+1 or n");
    DSPLIB_ASSERT(r.size() == n_, "output size must be n");

    const real_t dn = real_t(1) / n_;
    std::vector<cmplx_t> Z(n_ / 2);
    for (int i = 0; i < n_ / 2; ++i) {
        const auto v = x[n_ / 2 - i].conj();
        const cmplx_t Xe = (x[i] + v) * dn;
        const cmplx_t Xo = (x[i] - v) * dn * w_[i];
        Z[i].re = Xe.re - Xo.im;
        Z[i].im = -Xe.im - Xo.re;
    }

    const auto z = fft_->solve(Z);
    for (int i = 0; i < n_ / 2; ++i) {
        r[2 * i] = z[i].re;
        r[2 * i + 1] = -z[i].im;
    }
}

int RealIfftPlan::size() const noexcept {
    return n_;
}

}   // namespace dsplib