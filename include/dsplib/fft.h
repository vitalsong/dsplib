#pragma once

#include <dsplib/array.h>
#include <memory>

namespace dsplib {

//base complex value FFT plan
class FftPlanC
{
public:
    virtual ~FftPlanC() = default;
    [[nodiscard]] virtual arr_cmplx solve(const arr_cmplx& x) const = 0;
    virtual void solve(const cmplx_t* x, cmplx_t* y, int n) const {
        const auto r = this->solve(arr_cmplx(x, n));
        std::memcpy(y, r.data(), n * sizeof(cmplx_t));
    }
    [[nodiscard]] virtual int size() const noexcept = 0;
};

//base real value FFT plan
class FftPlanR
{
public:
    virtual ~FftPlanR() = default;
    [[nodiscard]] virtual arr_cmplx solve(const arr_real& x) const = 0;
    virtual void solve(const real_t* x, cmplx_t* y, int n) const {
        //TODO: use span
        const auto r = this->solve(arr_real(x, n));
        std::memcpy(y, r.data(), n * sizeof(cmplx_t));
    }
    [[nodiscard]] virtual int size() const noexcept = 0;
};

std::shared_ptr<FftPlanC> fft_plan_c(int n);
std::shared_ptr<FftPlanR> fft_plan_r(int n);

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
arr_cmplx fft(const arr_real& x, int n);

arr_cmplx rfft(const arr_real& x);          // equal `fft(x)`
arr_cmplx rfft(const arr_real& x, int n);   // equal `fft(x, n)`

}   // namespace dsplib
