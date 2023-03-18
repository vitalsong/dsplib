#pragma once

#include <dsplib/array.h>
#include <memory>

namespace dsplib {

class BaseFftPlan
{
public:
    virtual ~BaseFftPlan() = default;
    [[nodiscard]] virtual arr_cmplx solve(const arr_cmplx& x) const = 0;
    [[nodiscard]] virtual arr_cmplx solve(const arr_real& x) const = 0;
    [[nodiscard]] virtual int size() const noexcept = 0;
};

class FftPlan : public BaseFftPlan
{
public:
    explicit FftPlan(int n);

    arr_cmplx operator()(const arr_cmplx& x) const {
        return _d->solve(x);
    }

    arr_cmplx operator()(const arr_real& x) const {
        return _d->solve(arr_cmplx(x));
    }

    [[nodiscard]] arr_cmplx solve(const arr_cmplx& x) const final {
        return _d->solve(x);
    }

    [[nodiscard]] arr_cmplx solve(const arr_real& x) const final {
        return _d->solve(arr_cmplx(x));
    }

    [[nodiscard]] int size() const noexcept final {
        return _d->size();
    }

private:
    std::shared_ptr<BaseFftPlan> _d;
};

using fft_plan [[deprecated]] = FftPlan;

//TODO: rfft optimization

/*!
 * \brief Fast Fourier Transform (complex)
 * \details FFT for complex signal
 * \param arr Input array [N]
 * \return Result array [N]
 */
arr_cmplx fft(const arr_cmplx& x);

//n-point DFT
// if x.size() is less than n, then X is padded with trailing zeros to length n
// if x.size() is greater than n, then x is truncated to length n
arr_cmplx fft(const arr_cmplx& x, int n);

}   // namespace dsplib
