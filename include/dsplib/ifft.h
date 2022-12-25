#pragma once

#include <dsplib/types.h>
#include <dsplib/array.h>
#include <memory>

namespace dsplib {

class IfftPlanImpl;

class IfftPlan
{
public:
    IfftPlan(int n);
    arr_cmplx operator()(const arr_cmplx& x) const;
    arr_cmplx solve(const arr_cmplx& x) const;

private:
    std::shared_ptr<IfftPlanImpl> _d;
};

using ifft_plan [[deprecated]] = IfftPlan;

/*!
 * \brief Inverse fourier transform
 * \details IFFT for complex signal
 * \param arr Input array [N]
 * \return Result array [N]
 */
arr_cmplx ifft(const arr_cmplx& arr);

}   // namespace dsplib
