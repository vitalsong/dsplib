#include <dsplib/ifft.h>
#include <dsplib/fft.h>
#include <dsplib/math.h>

namespace dsplib {

namespace {

void _inplace_conj(arr_cmplx& x) {
    for (auto& v : x) {
        v.im = -v.im;
    }
}

std::vector<cmplx_t> _irfft_coeffs(int n) noexcept {
    assert(n % 4 == 0);
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

}   // namespace

//---------------------------------------------------------------------------------
IfftPlan::IfftPlan(int n)
  : _d{std::make_shared<FftPlan>(n)} {
}

arr_cmplx IfftPlan::operator()(const arr_cmplx& x) const {
    return this->solve(x);
}

arr_cmplx IfftPlan::solve(const arr_cmplx& x) const {
    const real_t m = real_t(1) / x.size();
    arr_cmplx y = x * m;
    _inplace_conj(y);
    y = _d->solve(y);
    _inplace_conj(y);
    return y;
}

int IfftPlan::size() const noexcept {
    return _d->size();
}

//---------------------------------------------------------------------------------
IfftPlanR::IfftPlanR(int n)
  : _n{n}
  , _d{std::make_shared<FftPlan>(n / 2)}
  , _w(_irfft_coeffs(n)) {
    DSPLIB_ASSERT(n % 2 == 0, "ifft size must be even");
}

arr_real IfftPlanR::operator()(const arr_cmplx& x) const {
    return this->solve(x);
}

arr_real IfftPlanR::solve(const arr_cmplx& x) const {
    assert(_n % 2 == 0);
    DSPLIB_ASSERT((x.size() == _n) || (x.size() == _n / 2 + 1), "input size must be n/2+1 or n");

    const real_t dn = real_t(1) / _n;
    std::vector<cmplx_t> Z(_n / 2);
    for (int i = 0; i < _n / 2; ++i) {
        const auto v = conj(x[_n / 2 - i]);
        const cmplx_t Xe = (x[i] + v) * dn;
        const cmplx_t Xo = (x[i] - v) * dn * _w[i];
        Z[i].re = Xe.re - Xo.im;
        Z[i].im = -Xe.im - Xo.re;
    }

    const auto z = _d->solve(Z);
    std::vector<real_t> r(_n);
    for (int i = 0; i < _n / 2; ++i) {
        r[2 * i] = z[i].re;
        r[2 * i + 1] = -z[i].im;
    }
    return r;
}

int IfftPlanR::size() const noexcept {
    return _n;
}

//---------------------------------------------------------------------------------
arr_cmplx ifft(const arr_cmplx& x) {
    IfftPlan plan(x.size());
    return plan(x);
}

arr_real irfft(const arr_cmplx& x) {
    return irfft(x, x.size());
}

arr_real irfft(const arr_cmplx& x, int n) {
    //FIXIT: _irfft_coeffs overhead, use cache
    IfftPlanR plan(n);
    return plan(x);
}

}   // namespace dsplib
