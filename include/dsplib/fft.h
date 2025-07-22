#pragma once

#include <dsplib/array.h>

#include <memory>

namespace dsplib {

/**
 * @brief FFT c2c base class
 */
class FftPlanC
{
public:
    virtual ~FftPlanC() = default;

    [[nodiscard]] virtual arr_cmplx solve(span_t<cmplx_t> x) const = 0;

    /**
     * @brief c2c FFT solve
     * @param x [in] input array[n]
     * @param r [out] result array[n]
     */
    virtual void solve(span_t<cmplx_t> x, mut_span_t<cmplx_t> r) const = 0;

    [[nodiscard]] virtual int size() const noexcept = 0;
};

/**
 * @brief FFT r2c base class
 */
class FftPlanR
{
public:
    virtual ~FftPlanR() = default;

    [[nodiscard]] virtual arr_cmplx solve(span_t<real_t> x) const = 0;

    /**
     * @brief r2c FFT solve
     * @param x [in] input array[n]
     * @param r [out] result array[n]
     */
    virtual void solve(span_t<real_t> x, mut_span_t<cmplx_t> r) const = 0;

    [[nodiscard]] virtual int size() const noexcept = 0;
};

/**
 * @param n FFT size
 * @return c2c FFT plan
 */
std::shared_ptr<FftPlanC> fft_plan_c(int n);

/**
 * @param n FFT size
 * @return r2c FFT plan
 */
std::shared_ptr<FftPlanR> fft_plan_r(int n);

/**
 * @brief Fast Fourier Transform (complex)
 * @details FFT for complex signal
 * @param arr Input array [N]
 * @return Result array [N]
 */
arr_cmplx fft(span_t<cmplx_t> x);

//n-point DFT
// if x.size() is less than n, then X is padded with trailing zeros to length n
// if x.size() is greater than n, then x is truncated to length n
arr_cmplx fft(span_t<cmplx_t> x, int n);

//Fast Fourier Transform (real)
arr_cmplx fft(span_t<real_t> x);
arr_cmplx fft(span_t<real_t> x, int n);

arr_cmplx rfft(span_t<real_t> x);          // equal `fft(x)`
arr_cmplx rfft(span_t<real_t> x, int n);   // equal `fft(x, n)`

}   // namespace dsplib
