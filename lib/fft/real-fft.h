#pragma once

#include <dsplib/fft.h>

namespace dsplib {

class RealFftPlan : public FftPlanR
{
public:
    explicit RealFftPlan(int n);
    [[nodiscard]] arr_cmplx solve(const arr_real& x) const final;
    [[nodiscard]] int size() const noexcept final;

private:
    const int n_;
    std::shared_ptr<FftPlanC> fft_;
    const std::vector<cmplx_t> w_;
};

}   // namespace dsplib