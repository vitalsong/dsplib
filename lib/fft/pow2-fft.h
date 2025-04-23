#pragma once

#include <dsplib/fft.h>

#include <cstdint>

namespace dsplib {

class Pow2FftPlan : public FftPlanC
{
public:
    explicit Pow2FftPlan(int n);
    [[nodiscard]] arr_cmplx solve(span_t<cmplx_t> x) const final;
    void solve(span_t<cmplx_t> x, mut_span_t<cmplx_t> r) const final;
    [[nodiscard]] int size() const noexcept final;

private:
    void _fft(const cmplx_t* in, cmplx_t* out, int n) const noexcept;

    const int n_;
    const int l_;
    std::vector<int32_t> bitrev_;
    std::vector<cmplx_t> coeffs_;
};

}   // namespace dsplib