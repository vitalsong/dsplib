#ifndef TYPES_H
#define TYPES_H

#include <vector>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
//base scalar type
typedef double real_t;

//-------------------------------------------------------------------------------------------------
//basic complex type
struct cmplx_t
{
    cmplx_t(real_t _re = 0, real_t _im = 0) noexcept
    {
        re = _re;
        im = _im;
    }

    cmplx_t(const cmplx_t&) = default;
    cmplx_t& operator = (const cmplx_t&) = default;

    real_t re;
    real_t im;

    template <typename T>
    cmplx_t& operator = (const T& rhs) noexcept
    {
        re = rhs;
        im = 0;
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
};

} ///< dsplib

#endif // TYPES_H
