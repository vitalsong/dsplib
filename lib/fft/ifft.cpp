#include <dsplib/ifft.h>
#include <dsplib/fft.h>
#include <dsplib/math.h>

#include "fft/factory.h"

namespace dsplib {

//---------------------------------------------------------------------------------
IfftPlan::IfftPlan(int n)
  : ifft_{create_ifft_plan(n)} {
}

arr_cmplx IfftPlan::operator()(const arr_cmplx& x) const {
    return this->solve(x);
}

arr_cmplx IfftPlan::solve(const arr_cmplx& x) const {
    return ifft_->solve(x);
}

int IfftPlan::size() const noexcept {
    return ifft_->size();
}

//---------------------------------------------------------------------------------
IfftPlanR::IfftPlanR(int n)
  : ifft_{create_irfft_plan(n)} {
}

arr_real IfftPlanR::operator()(const arr_cmplx& x) const {
    return this->solve(x);
}

arr_real IfftPlanR::solve(const arr_cmplx& x) const {
    return ifft_->solve(x);
}

int IfftPlanR::size() const noexcept {
    return ifft_->size();
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
    IfftPlanR plan(n);
    return plan(x);
}

}   // namespace dsplib
