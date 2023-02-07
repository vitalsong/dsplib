#pragma once

#include <dsplib/array.h>
#include <memory>

namespace dsplib {

class FftPlanImpl;

//Fast Fourier Transform calculation plan
class FftPlan
{
public:
    explicit FftPlan(int n);
    [[nodiscard]] arr_cmplx operator()(const arr_cmplx& x) const {
        return this->solve(x);
    }
    [[nodiscard]] arr_cmplx solve(const arr_cmplx& x) const;
    [[nodiscard]] int size() const noexcept;

private:
    std::shared_ptr<FftPlanImpl> _d;
};

//Inverse Fast Fourier Transform calculation plan
class IFftPlan : public FftPlan
{
public:
    explicit IFftPlan(int n)
      : FftPlan(n) {
    }

    [[nodiscard]] arr_cmplx operator()(const arr_cmplx& x) const {
        return this->solve(x);
    }
    [[nodiscard]] arr_cmplx solve(const arr_cmplx& x) const;
    [[nodiscard]] int size() const noexcept;
};

using fft_plan [[deprecated]] = FftPlan;
using ifft_plan [[deprecated]] = IFftPlan;

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

//Fast Fourier Transform (real)
arr_cmplx fft(const arr_real& x);

/*!
 * \brief Inverse fourier transform
 * \details IFFT for complex signal
 * \param x Input array [N]
 * \return Result array [N]
 */
arr_cmplx ifft(const arr_cmplx& x);

}   // namespace dsplib
