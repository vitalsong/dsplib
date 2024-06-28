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
    std::shared_ptr<FftPlan> _d;
};

class IfftPlanR
{
public:
    IfftPlanR(int n);
    arr_real operator()(const arr_cmplx& x) const;
    [[nodiscard]] arr_real solve(const arr_cmplx& x) const;
    [[nodiscard]] int size() const noexcept;

private:
    const int _n;
    std::shared_ptr<FftPlan> _d;
    const std::vector<cmplx_t> _w;   //exp(1i * 2 * pi / n)
};

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
