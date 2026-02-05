#pragma once

#include <dsplib/fft.h>
#include <dsplib/array.h>
#include <memory>

namespace dsplib {

using IfftPlanC = FftPlanC;

/**
 * @brief iFFT c2r base class
 */
class IfftPlanR
{
public:
    virtual ~IfftPlanR() = default;

    [[nodiscard]] virtual arr_real solve(span_t<cmplx_t> x) const = 0;

    /**
     * @brief c2r iFFT solve
     * @param x [in] input array[n]
     * @param r [out] result array[n]
     */
    virtual void solve(span_t<cmplx_t> x, mut_span_t<real_t> r) const {
        r = this->solve(x);
    }

    [[nodiscard]] virtual int size() const noexcept = 0;
};

/**
 * @param n iFFT size
 * @return c2c iFFT plan
 */
std::shared_ptr<IfftPlanC> ifft_plan_c(int n);

/**
 * @param n iFFT size
 * @return c2r iFFT plan
 */
std::shared_ptr<IfftPlanR> ifft_plan_r(int n);

/**
 * @brief Inverse fourier transform
 * @details IFFT for complex signal
 * @param x Input array [N]
 * @return Result array [N]
 */
arr_cmplx ifft(span_t<cmplx_t> x);

/**
 * @brief Inverse real fourier transform
 * @param x Input array [N or N/2+1]
 * @param n Transform size
 * @return Result array [N]
 */
arr_real irfft(span_t<cmplx_t> x, int n);

arr_real irfft(span_t<cmplx_t> x);

}   // namespace dsplib
