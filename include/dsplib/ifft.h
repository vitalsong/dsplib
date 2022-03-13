#pragma once

#include <dsplib/types.h>
#include <dsplib/array.h>
#include <memory>

namespace dsplib {

class ifft_plan_impl;

class ifft_plan
{
public:
    ifft_plan(int n);
    arr_cmplx operator()(const arr_cmplx& x) const;
    arr_cmplx solve(const arr_cmplx& x) const;

private:
    std::shared_ptr<ifft_plan_impl> _d;
};

/*!
 * \brief Inverse fourier transform
 * \details IFFT for complex signal
 * \param arr Input array [N]
 * \return Result array [N]
 */
arr_cmplx ifft(const arr_cmplx& arr);

}   // namespace dsplib
