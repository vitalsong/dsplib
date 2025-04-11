#pragma once

#include <dsplib/fft.h>
#include <dsplib/array.h>
#include <memory>

namespace dsplib {

using IfftPlanC = FftPlanC;

class IfftPlanR
{
public:
    virtual ~IfftPlanR() = default;
    [[nodiscard]] virtual arr_real solve(const arr_cmplx& x) const = 0;
    [[nodiscard]] virtual int size() const noexcept = 0;

    arr_real operator()(const arr_cmplx& x) const {
        return this->solve(x);
    }
};

std::shared_ptr<IfftPlanC> ifft_plan_c(int n);
std::shared_ptr<IfftPlanR> ifft_plan_r(int n);

/*!
 * \brief Inverse fourier transform
 * \details IFFT for complex signal
 * \param x Input array [N]
 * \return Result array [N]
 */
arr_cmplx ifft(const arr_cmplx& x);

/*!
 * \brief Inverse real fourier transform
 * \param x Input array [N or N/2+1]
 * \param n Transform size
 * \return Result array [N]
 */
arr_real irfft(const arr_cmplx& x, int n);

arr_real irfft(const arr_cmplx& x);

}   // namespace dsplib
