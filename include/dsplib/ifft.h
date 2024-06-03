#pragma once

#include <dsplib/fft.h>
#include <memory>

namespace dsplib {

class IfftPlan
{
public:
    IfftPlan(int n);
    arr_cmplx operator()(span_t<cmplx_t> x) const;
    [[nodiscard]] arr_cmplx solve(span_t<cmplx_t> x) const;
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
arr_cmplx ifft(span_t<cmplx_t> arr);

}   // namespace dsplib
