#pragma once

#include <dsplib/array.h>
#include <memory>

namespace dsplib {

class BaseIfftPlanC
{
public:
    virtual ~BaseIfftPlanC() = default;
    [[nodiscard]] virtual arr_cmplx solve(const arr_cmplx& x) const = 0;
    [[nodiscard]] virtual int size() const noexcept = 0;
};

class BaseIfftPlanR
{
public:
    virtual ~BaseIfftPlanR() = default;
    [[nodiscard]] virtual arr_real solve(const arr_cmplx& x) const = 0;
    [[nodiscard]] virtual int size() const noexcept = 0;
};

class IfftPlan : public BaseIfftPlanC
{
public:
    explicit IfftPlan(int n);
    arr_cmplx operator()(const arr_cmplx& x) const;
    [[nodiscard]] arr_cmplx solve(const arr_cmplx& x) const final;
    [[nodiscard]] int size() const noexcept final;

private:
    std::shared_ptr<BaseIfftPlanC> ifft_;
};

class IfftPlanR : public BaseIfftPlanR
{
public:
    explicit IfftPlanR(int n);
    arr_real operator()(const arr_cmplx& x) const;
    [[nodiscard]] arr_real solve(const arr_cmplx& x) const final;
    [[nodiscard]] int size() const noexcept final;

private:
    std::shared_ptr<BaseIfftPlanR> ifft_;
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
