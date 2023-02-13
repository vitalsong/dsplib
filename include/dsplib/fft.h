#pragma once

#include <dsplib/array.h>
#include <memory>

namespace dsplib {

class FftPlanImpl;

class FftPlan
{
public:
    FftPlan(int n);
    arr_cmplx operator()(const arr_cmplx& x) const;
    [[nodiscard]] arr_cmplx solve(const arr_cmplx& x) const;
    [[nodiscard]] int size() const noexcept;

private:
    std::shared_ptr<FftPlanImpl> _d;
};

using fft_plan [[deprecated]] = FftPlan;

//TODO: rfft optimization

/*!
 * \brief Fast Fourier Transform (complex)
 * \details FFT for complex signal
 * \param arr Input array [N]
 * \return Result array [N]
 */
arr_cmplx fft(const arr_cmplx& x);

//n-point DFT
// if x.size() is less than n, then X is padded with trailing zeros to length n
// if x.size() is greater than n, then x is truncated to length n
arr_cmplx fft(const arr_cmplx& x, int n);

}   // namespace dsplib
