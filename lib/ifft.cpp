#include <dsplib/ifft.h>
#include <dsplib/fft.h>
#include <dsplib/math.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
IfftPlan::IfftPlan(int n)
  : _d{std::make_shared<FftPlan>(n)} {
}

//-------------------------------------------------------------------------------------------------
arr_cmplx IfftPlan::operator()(const arr_cmplx& x) const {
    return this->solve(x);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx IfftPlan::solve(const arr_cmplx& x) const {
    const int n = x.size();
    return conj(_d->solve(conj(x)) / n);
}

//-------------------------------------------------------------------------------------------------
int IfftPlan::size() const noexcept {
    return _d->size();
}

//-------------------------------------------------------------------------------------------------
arr_cmplx ifft(const arr_cmplx& x) {
    IfftPlan plan(x.size());
    return plan(x);
}

}   // namespace dsplib
