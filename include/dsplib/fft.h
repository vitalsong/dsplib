#pragma once

#include <dsplib/array.h>
#include <memory>

namespace dsplib {

class fft_plan_impl;

class fft_plan
{
public:
    fft_plan(int n);
    arr_cmplx operator()(const arr_cmplx& x) const;
    arr_cmplx solve(const arr_cmplx& x) const;

private:
    std::shared_ptr<fft_plan_impl> _d;
};

//TODO: rfft optimization

/*!
 * \brief Fast Fourier Transform (complex)
 * \details FFT for complex signal
 * \param arr Input array [N]
 * \return Result array [N]
 */
arr_cmplx fft(const arr_cmplx& x);

}   // namespace dsplib
