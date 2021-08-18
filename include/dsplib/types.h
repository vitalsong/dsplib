#pragma once

#include <complex>

#ifndef restrict
#ifdef _MSC_VER
#define restrict __restrict
#else
#define restrict __restrict__
#endif
#endif

// fix for interger real (because 5+5i is not compiled, but 5.0+5i is OK)
//-------------------------------------------------------------------------------------------------
inline std::complex<double> operator + (const int& lhs, const std::complex<double>& rhs) {
    return std::complex<double>(double(lhs)) + rhs;
}

//-------------------------------------------------------------------------------------------------
inline std::complex<double> operator - (const int& lhs, const std::complex<double>& rhs) {
    return std::complex<double>(double(lhs)) - rhs;
}

//-------------------------------------------------------------------------------------------------
inline std::complex<double> operator + (const std::complex<double>& lhs, const int& rhs) {
    return lhs + std::complex<double>(double(rhs));
}

//-------------------------------------------------------------------------------------------------
inline std::complex<double> operator - (const std::complex<double>& lhs, const int& rhs) {
    return lhs - std::complex<double>(double(rhs));
}

namespace dsplib {

constexpr double pi = 3.141592653589793238463;

//-------------------------------------------------------------------------------------------------
//base scalar type
typedef double real_t;

//-------------------------------------------------------------------------------------------------
//basic complex type
struct cmplx_t
{
    constexpr cmplx_t(real_t _re = 0, real_t _im = 0) : re(_re), im(_im){}
    constexpr cmplx_t(const std::complex<real_t>& v) : re(v.real()), im(v.imag()){}
    constexpr cmplx_t(const cmplx_t&) = default;

    constexpr operator std::complex<real_t>() const {
        return std::complex<real_t>(re, im);
    }

    real_t re;
    real_t im;

    cmplx_t& operator = (const cmplx_t&) = default;

    template <typename T>
    cmplx_t& operator = (const T& rhs) noexcept
    {
        re = rhs;
        im = 0;
        return *this;
    }

    cmplx_t& operator = (const std::complex<double>& v) noexcept
    {
        re = v.real();
        im = v.imag();
        return *this;
    }

    cmplx_t& operator + () noexcept {
        return *this;
    }

    cmplx_t& operator - () noexcept
    {
        re = -re;
        im = -im;
        return *this;
    }

    cmplx_t& operator += (const cmplx_t& rhs) noexcept
    {
        re += rhs.re;
        im += rhs.im;
        return *this;
    }

    cmplx_t& operator -= (const cmplx_t& rhs) noexcept
    {
        re -= rhs.re;
        im -= rhs.im;
        return *this;
    }

    cmplx_t& operator *= (const cmplx_t& rhs) noexcept
    {
        real_t ti = (re * rhs.re) - (im * rhs.im);
        im = (re * rhs.im) + (im * rhs.re);
        re = ti;
        return *this;
    }

    cmplx_t& operator /= (const cmplx_t& rhs) noexcept
    {
        real_t b = (rhs.re * rhs.re) + (rhs.im * rhs.im);
        real_t ti = ((re * rhs.re) + (im * rhs.im)) / b;
        im = ((rhs.re * im) - (re * rhs.im)) / b;
        re = ti;
        return *this;
    }

    cmplx_t operator + (const cmplx_t &rhs) const noexcept
    {
        cmplx_t tmp = *this;
        tmp += rhs;
        return tmp;
    }

    cmplx_t operator - (const cmplx_t &rhs) const noexcept
    {
        cmplx_t tmp = *this;
        tmp -= rhs;
        return tmp;
    }

    cmplx_t operator * (const cmplx_t &rhs) const noexcept
    {
        cmplx_t tmp = *this;
        tmp *= rhs;
        return tmp;
    }

    cmplx_t operator / (const cmplx_t &rhs) const noexcept
    {
        cmplx_t tmp = *this;
        tmp /= rhs;
        return tmp;
    }

    cmplx_t& operator *= (const real_t& rhs) noexcept
    {
        re = (re * rhs);
        im = (im * rhs);
        return *this;
    }

    cmplx_t operator * (const real_t& rhs) const noexcept
    {
        cmplx_t tmp = *this;
        tmp *= rhs;
        return tmp;
    }

    cmplx_t& operator /= (const real_t& rhs) noexcept
    {
        re = (re / rhs);
        im = (im / rhs);
        return *this;
    }

    cmplx_t operator / (const real_t& rhs) const noexcept
    {
        cmplx_t tmp = *this;
        tmp /= rhs;
        return tmp;
    }

    bool operator > (const cmplx_t& rhs) const noexcept {
        return (re * re + im * im) > (rhs.re * rhs.re + rhs.im * rhs.im);
    }

    bool operator < (const cmplx_t& rhs) const noexcept {
        return (re * re + im * im) < (rhs.re * rhs.re + rhs.im * rhs.im);
    }

    cmplx_t conj() const {
        return {re, -im};
    }
};

inline cmplx_t operator + (const real_t& lhs, const cmplx_t& rhs) {
    return rhs + lhs;
}

inline cmplx_t operator - (const real_t& lhs, const cmplx_t& rhs) {
    cmplx_t r(rhs);
    return -r + lhs;
}

inline cmplx_t operator * (const real_t& lhs, const cmplx_t& rhs) {
    return rhs * lhs;
}

inline cmplx_t operator / (const real_t& lhs, const cmplx_t& rhs) {
    cmplx_t r(rhs);
    r.re = lhs / r.re;
    r.im = lhs / r.im;
    return r;
}

} ///< dsplib
