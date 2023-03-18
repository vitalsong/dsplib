#pragma once

#include <dsplib/types.h>
#include <dsplib/array.h>
#include <dsplib/fft.h>
#include <memory>

namespace dsplib {

class IfftPlan
{
public:
    IfftPlan(int n);
    arr_cmplx operator()(const arr_cmplx& x) const;
    [[nodiscard]] arr_cmplx solve(const arr_cmplx& x) const;
    [[nodiscard]] int size() const noexcept;

private:
    std::shared_ptr<BaseFftPlanC> _d;
};

/*!
 * \brief Inverse fourier transform
 * \details IFFT for complex signal
 * \param arr Input array [N]
 * \return Result array [N]
 */
arr_cmplx ifft(const arr_cmplx& arr);

}   // namespace dsplib
