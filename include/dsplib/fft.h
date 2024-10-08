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
    virtual void solve(const real_t* x, cmplx_t* y, int n) const {
        //TODO: use span
        const auto r = this->solve(arr_real(x, n));
        std::memcpy(y, r.data(), n * sizeof(cmplx_t));
    }
    [[nodiscard]] virtual int size() const noexcept = 0;
};

//base real value FFT plan
class BaseFftPlanC
{
public:
    virtual ~BaseFftPlanC() = default;
    [[nodiscard]] virtual arr_cmplx solve(const arr_cmplx& x) const = 0;
    virtual void solve(const cmplx_t* x, cmplx_t* y, int n) const {
        const auto r = this->solve(arr_cmplx(x, n));
        std::memcpy(y, r.data(), n * sizeof(cmplx_t));
    }
    [[nodiscard]] virtual int size() const noexcept = 0;
};

//complex value FFT plan
class FftPlan : public BaseFftPlanC
{
public:
    explicit FftPlan(int n);

    arr_cmplx operator()(const arr_cmplx& x) const {
        return this->solve(x);
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

//real value FFT plan
class FftPlanR : public BaseFftPlanR
{
public:
    explicit FftPlanR(int n);

    arr_cmplx operator()(const arr_real& x) const {
        return this->solve(x);
    }

    [[nodiscard]] arr_cmplx solve(const arr_real& x) const final {
        return _d->solve(x);
    }

    [[nodiscard]] int size() const noexcept final {
        return _d->size();
    }

private:
    std::shared_ptr<BaseFftPlanR> _d;
};

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

//Fast Fourier Transform (real)
arr_cmplx fft(const arr_real& x);
arr_cmplx fft(const arr_real& x, int n);

arr_cmplx rfft(const arr_real& x);          // equal `fft(x)`
arr_cmplx rfft(const arr_real& x, int n);   // equal `fft(x, n)`

}   // namespace dsplib
