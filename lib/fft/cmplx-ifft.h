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
        arr_cmplx r(x);
        this->solve(x, r);
        return r;
    }

    void solve(span_t<cmplx_t> x, mut_span_t<cmplx_t> r) const final {
        const int n = fft_->size();
        DSPLIB_ASSERT(x.size() == n, "array size error");
        DSPLIB_ASSERT(x.size() == r.size(), "array size error");
        arr_cmplx t(n);
        const real_t m = real_t(1) / n;
        for (int i = 0; i < n; ++i) {
            t[i].re = x[i].re * m;
            t[i].im = -(x[i].im * m);
        }
        fft_->solve(t, r);
        for (int i = 0; i < n; ++i) {
            r[i].im = -r[i].im;
        }
    }

    int size() const noexcept final {
        return fft_->size();
    }

private:
    std::shared_ptr<FftPlanC> fft_;
};

}   // namespace dsplib