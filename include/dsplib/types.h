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
    cmplx_t(real_t _xi = 0, real_t _xq = 0) noexcept
    {
        xi = _xi;
        xq = _xq;
    }

    cmplx_t(const cmplx_t&) = default;
    cmplx_t& operator = (const cmplx_t&) = default;

    real_t xi;
    real_t xq;

    template <typename T>
    cmplx_t& operator = (const T& rhs) noexcept
    {
        xi = rhs;
        xq = 0;
        return *this;
    }

    cmplx_t& operator + () noexcept {
        return *this;
    }

    cmplx_t& operator - () noexcept
    {
        xi = -xi;
        xq = -xq;
        return *this;
    }

    cmplx_t& operator += (const cmplx_t& rhs) noexcept
    {
        xi += rhs.xi;
        xq += rhs.xq;
        return *this;
    }

    cmplx_t& operator -= (const cmplx_t& rhs) noexcept
    {
        xi -= rhs.xi;
        xq -= rhs.xq;
        return *this;
    }

    cmplx_t& operator *= (const cmplx_t& rhs) noexcept
    {
        real_t ti = (xi * rhs.xi) - (xq * rhs.xq);
        xq = (xi * rhs.xq) + (xq * rhs.xi);
        xi = ti;
        return *this;
    }

    cmplx_t& operator /= (const cmplx_t& rhs) noexcept
    {
        real_t b = (rhs.xi * rhs.xi) + (rhs.xq * rhs.xq);
        real_t ti = ((xi * rhs.xi) + (xq * rhs.xq)) / b;
        xq = ((rhs.xi * xq) - (xi * rhs.xq)) / b;
        xi = ti;
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
        xi = (xi * rhs);
        xq = (xq * rhs);
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
        xi = (xi / rhs);
        xq = (xq / rhs);
        return *this;
    }

    cmplx_t operator / (const real_t& rhs) const noexcept
    {
        cmplx_t tmp = *this;
        tmp /= rhs;
        return tmp;
    }

    bool operator > (const cmplx_t& rhs) const noexcept {
        return (xi * xi + xq * xq) > (rhs.xi * rhs.xi + rhs.xq * rhs.xq);
    }

    bool operator < (const cmplx_t& rhs) const noexcept {
        return (xi * xi + xq * xq) < (rhs.xi * rhs.xi + rhs.xq * rhs.xq);
    }
};

} ///< dsplib

#endif // TYPES_H
