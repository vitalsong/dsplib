#pragma once

#include <dsplib/array.h>

#include <memory>

namespace dsplib {

//base complex value FFT plan
class BaseFftPlanR
{
public:
    virtual ~BaseFftPlanR() = default;

    [[nodiscard]] virtual arr_cmplx solve(span_t<real_t> x) const = 0;

    virtual void solve(span_t<real_t> x, mut_span_t<cmplx_t> y) const {
        DSPLIB_ASSERT(x.size() == y.size(), "sizes must be equal");
        y = this->solve(x);
    }

    [[nodiscard]] virtual int size() const noexcept = 0;
};

//base real value FFT plan
class BaseFftPlanC
{
public:
    virtual ~BaseFftPlanC() = default;

    [[nodiscard]] virtual arr_cmplx solve(span_t<cmplx_t> x) const = 0;

    virtual void solve(span_t<cmplx_t> x, mut_span_t<cmplx_t> y) const {
        DSPLIB_ASSERT(x.size() == y.size(), "sizes must be equal");
        y = this->solve(x);
    }

    [[nodiscard]] virtual int size() const noexcept = 0;
};

//complex value FFT plan
class FftPlan : public BaseFftPlanC
{
public:
    explicit FftPlan(int n);

    arr_cmplx operator()(span_t<cmplx_t> x) const {
        return this->solve(x);
    }

    [[nodiscard]] arr_cmplx solve(span_t<cmplx_t> x) const final {
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

    arr_cmplx operator()(span_t<real_t> x) const {
        return this->solve(x);
    }

    [[nodiscard]] arr_cmplx solve(span_t<real_t> x) const final {
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
 * \param x Input array [N]
 * \return Result array [N]
 */
arr_cmplx fft(span_t<cmplx_t> x);

//n-point DFT
// if x.size() is less than n, then X is padded with trailing zeros to length n
// if x.size() is greater than n, then x is truncated to length n
arr_cmplx fft(span_t<cmplx_t> x, int n);

//Fast Fourier Transform (real)
arr_cmplx fft(span_t<real_t> x);
arr_cmplx fft(span_t<real_t> x, int n);

}   // namespace dsplib
