#pragma once

#include <dsplib/fft.h>

#include <cstdint>

namespace dsplib {

class Pow2FftPlan : public BaseFftPlanC
{
public:
    explicit Pow2FftPlan(int n);

    [[nodiscard]] arr_cmplx solve(const arr_cmplx& x) const final;

    void solve(const cmplx_t* x, cmplx_t* y, int n) const final;

    [[nodiscard]] int size() const noexcept final;

private:
    void _fft(const cmplx_t* in, cmplx_t* out, int n) const noexcept;

    const int n_;
    const int l_;
    std::vector<int32_t> bitrev_;
    std::vector<cmplx_t> coeffs_;
};

}   // namespace dsplib