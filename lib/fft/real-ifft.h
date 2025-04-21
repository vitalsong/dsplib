#pragma once

#include <dsplib/ifft.h>
#include <dsplib/fft.h>

namespace dsplib {

class RealIfftPlan : public IfftPlanR
{
public:
    explicit RealIfftPlan(int n);
    [[nodiscard]] arr_real solve(const arr_cmplx& x) const final;
    [[nodiscard]] int size() const noexcept final;

private:
    const int n_;
    std::shared_ptr<FftPlanC> fft_;
    const std::vector<cmplx_t> w_;   //exp(1i * 2 * pi / n)
};

}   // namespace dsplib