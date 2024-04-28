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
}   // namespace

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

arr_cmplx ifft(const arr_cmplx& x) {
    IfftPlan plan(x.size());
    return plan(x);
}

}   // namespace dsplib
