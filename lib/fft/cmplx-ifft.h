#pragma once

#include <dsplib/ifft.h>

#include "fft/factory.h"

namespace dsplib {

class CmplxIfftPlan : public BaseIfftPlanC
{
public:
    explicit CmplxIfftPlan(int n)
      : fft_{create_fft_plan(n)} {
    }

    arr_cmplx solve(const arr_cmplx& x) const final {
        const real_t m = real_t(1) / x.size();
        arr_cmplx y = x * m;
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

    std::shared_ptr<BaseFftPlanC> fft_;
};

}   // namespace dsplib