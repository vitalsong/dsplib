#pragma once

#include <dsplib/fft.h>

namespace dsplib {

class RealFftPlan : public BaseFftPlanR
{
public:
    explicit RealFftPlan(int n);
    [[nodiscard]] arr_cmplx solve(const arr_real& x) const final;
    [[nodiscard]] int size() const noexcept final;

private:
    const int n_;
    std::shared_ptr<BaseFftPlanC> fft_;
    const std::vector<cmplx_t> w_;
};

}   // namespace dsplib