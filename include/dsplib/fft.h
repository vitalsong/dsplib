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

//fft factorization to (2, 3, 5)
class FactorFFTPlan : public BaseFftPlanC
{
public:
    explicit FactorFFTPlan(int n);
    ~FactorFFTPlan() = default;
    [[nodiscard]] arr_cmplx solve(const arr_cmplx& x) const final;
    [[nodiscard]] int size() const noexcept final;

private:
    void _dft(cmplx_t* x, int n) const noexcept;
    void _fft(cmplx_t* x, int n, int dp) const noexcept;
    void _update_table(int p, int q) const noexcept;

    int _n;
    arr_int _factors;
    arr_cmplx _twiddle;
    mutable std::vector<cmplx_t> _tw;   ///< cached twiddle table
    mutable arr_cmplx _px;              ///< tmp matrix for transpose
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

}   // namespace dsplib
