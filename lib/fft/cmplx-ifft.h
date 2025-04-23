#pragma once

#include <dsplib/ifft.h>

namespace dsplib {

class CmplxIfftPlan : public IfftPlanC
{
public:
    explicit CmplxIfftPlan(int n)
      : fft_{fft_plan_c(n)} {
    }

    arr_cmplx solve(span_t<cmplx_t> x) const final {
        const real_t m = real_t(1) / x.size();
        arr_cmplx y(x);
        y *= m;
        _inplace_conj(y);
        y = fft_->solve(y);
        _inplace_conj(y);
        return y;
    }

    int size() const noexcept final {
        return fft_->size();
    }

private:
    static void _inplace_conj(arr_cmplx& x) {
        for (auto& v : x) {
            v.im = -v.im;
        }
    }

    std::shared_ptr<FftPlanC> fft_;
};

}   // namespace dsplib