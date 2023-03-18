#pragma once

#include <dsplib/array.h>
#include <memory>

namespace dsplib {

//base complex value FFT plan
class BaseFftPlanR
{
public:
    virtual ~BaseFftPlanR() = default;
    [[nodiscard]] virtual arr_cmplx solve(const arr_real& x) const = 0;
    [[nodiscard]] virtual int size() const noexcept = 0;
};

//base real value FFT plan
class BaseFftPlanC
{
public:
    virtual ~BaseFftPlanC() = default;
    [[nodiscard]] virtual arr_cmplx solve(const arr_cmplx& x) const = 0;
    [[nodiscard]] virtual int size() const noexcept = 0;
};

//complex value FFT plan
class FftPlan : public BaseFftPlanC
{
public:
    explicit FftPlan(int n);

    arr_cmplx operator()(const arr_cmplx& x) const {
        return _d->solve(x);
    }

    [[nodiscard]] arr_cmplx solve(const arr_cmplx& x) const final {
        return _d->solve(x);
    }

    [[nodiscard]] int size() const noexcept final {
        return _d->size();
    }

private:
    std::shared_ptr<BaseFftPlanC> _d;
};

/*!
 * \brief Fast Fourier Transform (complex)
 * \details FFT for complex signal
 * \param arr Input array [N]
 * \return Result array [N]
 */
arr_cmplx fft(const arr_cmplx& x);

//TODO: arr_cmplx fft(const arr_real& x);

//n-point DFT
// if x.size() is less than n, then X is padded with trailing zeros to length n
// if x.size() is greater than n, then x is truncated to length n
arr_cmplx fft(const arr_cmplx& x, int n);

}   // namespace dsplib
