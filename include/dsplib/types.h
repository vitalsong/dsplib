#pragma once

#include <complex>
#include <functional>
#include <optional>
#include <type_traits>

#ifndef restrict
#ifdef _MSC_VER
#define restrict __restrict
#else
#define restrict __restrict__
#endif
#endif

// fix for interger real (because 5+5i is not compiled, but 5.0+5i is OK)
//-------------------------------------------------------------------------------------------------
inline std::complex<double> operator+(const int& lhs, const std::complex<double>& rhs) {
    return std::complex<double>(double(lhs)) + rhs;
}

//-------------------------------------------------------------------------------------------------
inline std::complex<double> operator-(const int& lhs, const std::complex<double>& rhs) {
    return std::complex<double>(double(lhs)) - rhs;
}

//-------------------------------------------------------------------------------------------------
inline std::complex<double> operator+(const std::complex<double>& lhs, const int& rhs) {
    return lhs + std::complex<double>(double(rhs));
}

//-------------------------------------------------------------------------------------------------
inline std::complex<double> operator-(const std::complex<double>& lhs, const int& rhs) {
    return lhs - std::complex<double>(double(rhs));
}

namespace dsplib {

constexpr double pi = 3.141592653589793238463;

//-------------------------------------------------------------------------------------------------
//base scalar type
using real_t = double;

struct cmplx_t;

template<typename T>
struct is_scalar_ar : std::integral_constant<bool, std::is_arithmetic<T>::value || std::is_same<T, cmplx_t>::value>
{};

template<typename T>
using enable_scalar_t = std::enable_if<is_scalar_ar<T>::value>;

template<typename T, typename T2>
using enable_convertible_t = std::enable_if<std::is_convertible<T, T2>::value>;

template<typename T, class = void>
struct cmplx_convert_to_t : std::false_type
{};

template<typename T>
struct cmplx_convert_to_t<T, typename enable_scalar_t<T>::type>;

template<typename T>
constexpr cmplx_t to_cmplx_cast(const T& v);

template<typename T>
struct cmplx_convert_from_t : std::false_type
{};

template<typename T>
constexpr T from_cmplx_cast(const cmplx_t& v);

//-------------------------------------------------------------------------------------------------
//basic complex type
struct cmplx_t
{
    template<typename T>
    using convert_fn_t = std::function<cmplx_t(T)>;

    constexpr cmplx_t(real_t _re = 0, real_t _im = 0)
      : re(_re)
      , im(_im) {
    }
    constexpr cmplx_t(const cmplx_t&) = default;

    //scalar -> cmplx_t
    template<typename T, class _S = typename std::is_arithmetic<T>::type>
    constexpr cmplx_t(const T& v)
      : re{static_cast<real_t>(v)}
      , im{0} {
    }

    //std::complex -> cmplx_t
    template<typename T>
    cmplx_t(const std::complex<T>& v) {
        *this = to_cmplx_cast(v);
    }

    template<typename T, typename _C = typename std::enable_if<cmplx_convert_from_t<T>::value>::type>
    operator T() const {
        return from_cmplx_cast<T>(*this);
    }

    real_t re{0};
    real_t im{0};

    cmplx_t& operator=(const cmplx_t&) = default;

    cmplx_t& operator+() noexcept {
        return *this;
    }

    cmplx_t& operator-() noexcept {
        re = -re;
        im = -im;
        return *this;
    }

    cmplx_t& operator+=(const cmplx_t& rhs) noexcept {
        re += rhs.re;
        im += rhs.im;
        return *this;
    }

    cmplx_t& operator-=(const cmplx_t& rhs) noexcept {
        re -= rhs.re;
        im -= rhs.im;
        return *this;
    }

    cmplx_t& operator*=(const cmplx_t& rhs) noexcept {
        real_t ti = (re * rhs.re) - (im * rhs.im);
        im = (re * rhs.im) + (im * rhs.re);
        re = ti;
        return *this;
    }

    cmplx_t& operator/=(const cmplx_t& rhs) noexcept {
        real_t b = (rhs.re * rhs.re) + (rhs.im * rhs.im);
        real_t ti = ((re * rhs.re) + (im * rhs.im)) / b;
        im = ((rhs.re * im) - (re * rhs.im)) / b;
        re = ti;
        return *this;
    }

    cmplx_t operator+(const cmplx_t& rhs) const noexcept {
        cmplx_t tmp = *this;
        tmp += rhs;
        return tmp;
    }

    cmplx_t operator-(const cmplx_t& rhs) const noexcept {
        cmplx_t tmp = *this;
        tmp -= rhs;
        return tmp;
    }

    cmplx_t operator*(const cmplx_t& rhs) const noexcept {
        cmplx_t tmp = *this;
        tmp *= rhs;
        return tmp;
    }

    cmplx_t operator/(const cmplx_t& rhs) const noexcept {
        cmplx_t tmp = *this;
        tmp /= rhs;
        return tmp;
    }

    cmplx_t& operator*=(const real_t& rhs) noexcept {
        re = (re * rhs);
        im = (im * rhs);
        return *this;
    }

    cmplx_t operator*(const real_t& rhs) const noexcept {
        cmplx_t tmp = *this;
        tmp *= rhs;
        return tmp;
    }

    cmplx_t& operator/=(const real_t& rhs) noexcept {
        re = (re / rhs);
        im = (im / rhs);
        return *this;
    }

    cmplx_t operator/(const real_t& rhs) const noexcept {
        cmplx_t tmp = *this;
        tmp /= rhs;
        return tmp;
    }

    bool operator>(const cmplx_t& rhs) const noexcept {
        return (re * re + im * im) > (rhs.re * rhs.re + rhs.im * rhs.im);
    }

    bool operator<(const cmplx_t& rhs) const noexcept {
        return (re * re + im * im) < (rhs.re * rhs.re + rhs.im * rhs.im);
    }

    cmplx_t conj() const {
        return {re, -im};
    }
};

//left oriented real * cmplx
template<class T, class _S = typename enable_scalar_t<T>::type,
         class _C = typename enable_convertible_t<T, cmplx_t>::type>
inline cmplx_t operator+(const T& lhs, const cmplx_t& rhs) {
    return rhs + lhs;
}

template<class T, class _S = typename enable_scalar_t<T>::type,
         class _C = typename enable_convertible_t<T, cmplx_t>::type>
inline cmplx_t operator-(const T& lhs, const cmplx_t& rhs) {
    cmplx_t r(rhs);
    return -r + lhs;
}

template<class T, class _S = typename enable_scalar_t<T>::type,
         class _C = typename enable_convertible_t<T, cmplx_t>::type>
inline cmplx_t operator*(const T& lhs, const cmplx_t& rhs) {
    return rhs * lhs;
}

template<class T, class _S = typename enable_scalar_t<T>::type,
         class _C = typename enable_convertible_t<T, cmplx_t>::type>
inline cmplx_t operator/(const T& lhs, const cmplx_t& rhs) {
    return cmplx_t(lhs) / rhs;
}

//cast rules
template<typename T>
struct cmplx_convert_to_t<T, typename enable_scalar_t<T>::type> : std::true_type
{
    static cmplx_t cast(const T& v) {
        return cmplx_t{static_cast<real_t>(v), 0};
    }
};

template<typename T>
struct cmplx_convert_to_t<std::complex<T>> : std::true_type
{
    static cmplx_t cast(const std::complex<T>& v) {
        return cmplx_t{static_cast<real_t>(v.real()), static_cast<real_t>(v.imag())};
    }
};

template<typename T>
struct cmplx_convert_from_t<std::complex<T>> : std::true_type
{
    static std::complex<T> cast(const cmplx_t& v) {
        return std::complex<T>{static_cast<T>(v.re), static_cast<T>(v.im)};
    }
};

template<typename T>
constexpr cmplx_t to_cmplx_cast(const T& v) {
    static_assert(cmplx_convert_to_t<T>::value, "Type is not convertable. Please define 'cmplx_convert_to_t<T>'");
    return cmplx_convert_to_t<T>::cast(v);
}

template<typename T>
constexpr T from_cmplx_cast(const cmplx_t& v) {
    static_assert(cmplx_convert_from_t<T>::value, "Type is not convertable. Please define 'cmplx_convert_from_t<T>'");
    return cmplx_convert_from_t<T>::cast(v);
}

}   // namespace dsplib
