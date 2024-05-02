#pragma once

#include <complex>
#include <functional>
#include <type_traits>
#include <iosfwd>

#ifndef restrict
#ifdef _MSC_VER
#define restrict __restrict
#else
#define restrict __restrict__
#endif
#endif

// fix for interger real (because 5+5i is not compiled, but 5.0+5i is OK)
//-------------------------------------------------------------------------------------------------
template<typename T>
constexpr std::complex<T> operator+(const int& lhs, const std::complex<T>& rhs) {
    return std::complex<T>(T(lhs)) + rhs;
}

//-------------------------------------------------------------------------------------------------
template<typename T>
constexpr std::complex<T> operator-(const int& lhs, const std::complex<T>& rhs) {
    return std::complex<T>(T(lhs)) - rhs;
}

//-------------------------------------------------------------------------------------------------
template<typename T>
constexpr std::complex<T> operator+(const std::complex<T>& lhs, const int& rhs) {
    return lhs + std::complex<T>(T(rhs));
}

//-------------------------------------------------------------------------------------------------
template<typename T>
constexpr std::complex<T> operator-(const std::complex<T>& lhs, const int& rhs) {
    return lhs - std::complex<T>(T(rhs));
}

namespace dsplib {

//-------------------------------------------------------------------------------------------------
//base scalar type
#ifdef DSPLIB_USE_FLOAT32
using real_t = float;
#else
using real_t = double;
#endif

constexpr real_t pi = 3.141592653589793238463;

struct cmplx_t;

//Floating-point relative accuracy
double eps(float v);
double eps(double v);
real_t eps();

template<typename T>
struct is_scalar : std::integral_constant<bool, std::is_arithmetic_v<T> || std::is_same_v<T, cmplx_t>>
{};

template<typename T>
constexpr bool is_scalar_v = is_scalar<T>::value;

template<typename T>
using enable_scalar_t = std::enable_if<is_scalar_v<T>>;

template<typename T, typename T2>
using enable_convertible_t = std::enable_if<std::is_convertible_v<T, T2>>;

template<typename T1, typename T2>
using enable_same_t = std::enable_if<std::is_same_v<T1, T2>>;

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

    constexpr cmplx_t(real_t re_ = 0, real_t im_ = 0)
      : re(re_)
      , im(im_) {
    }

    constexpr cmplx_t(const cmplx_t&) = default;

    //scalar -> cmplx_t
    template<typename T, class S_ = typename std::is_arithmetic<T>::type>
    constexpr cmplx_t(const T& v)
      : re{static_cast<real_t>(v)} {
    }

    //std::complex -> cmplx_t
    template<typename T>
    cmplx_t(const std::complex<T>& v) {
        *this = to_cmplx_cast(v);
    }

    template<typename T, typename C_ = typename std::enable_if<cmplx_convert_from_t<T>::value>::type>
    operator T() const {
        return from_cmplx_cast<T>(*this);
    }

    real_t re{0};
    real_t im{0};

    cmplx_t& operator=(const cmplx_t&) = default;

    const cmplx_t& operator+() const noexcept {
        return *this;
    }

    cmplx_t operator-() const noexcept {
        return {-re, -im};
    }

    cmplx_t& operator+=(const cmplx_t& rhs) noexcept {
        *this = *this + rhs;
        return *this;
    }

    cmplx_t& operator-=(const cmplx_t& rhs) noexcept {
        *this = *this - rhs;
        return *this;
    }

    cmplx_t& operator*=(const cmplx_t& rhs) noexcept {
        *this = *this * rhs;
        return *this;
    }

    cmplx_t& operator/=(const cmplx_t& rhs) noexcept {
        *this = *this / rhs;
        return *this;
    }

    constexpr cmplx_t operator+(const cmplx_t& rhs) const noexcept {
        return {re + rhs.re, im + rhs.im};
    }

    constexpr cmplx_t operator-(const cmplx_t& rhs) const noexcept {
        return {re - rhs.re, im - rhs.im};
    }

    constexpr cmplx_t operator*(const cmplx_t& rhs) const noexcept {
        return {(re * rhs.re) - (im * rhs.im), (re * rhs.im) + (im * rhs.re)};
    }

    constexpr cmplx_t operator/(const cmplx_t& rhs) const noexcept {
        return {((re * rhs.re) + (im * rhs.im)) / rhs.abs2(), ((rhs.re * im) - (re * rhs.im)) / rhs.abs2()};
    }

    //cmplx * scalar
    cmplx_t& operator+=(const real_t& rhs) noexcept {
        re += rhs;
        return *this;
    }

    constexpr cmplx_t operator+(const real_t& rhs) const noexcept {
        return {re + rhs, im};
    }

    cmplx_t& operator-=(const real_t& rhs) noexcept {
        re -= rhs;
        return *this;
    }

    constexpr cmplx_t operator-(const real_t& rhs) const noexcept {
        return {re - rhs, im};
    }

    cmplx_t& operator*=(const real_t& rhs) noexcept {
        re *= rhs;
        im *= rhs;
        return *this;
    }

    constexpr cmplx_t operator*(const real_t& rhs) const noexcept {
        return {re * rhs, im * rhs};
    }

    cmplx_t& operator/=(const real_t& rhs) noexcept {
        re = (re / rhs);
        im = (im / rhs);
        return *this;
    }

    constexpr cmplx_t operator/(const real_t& rhs) const noexcept {
        return {re / rhs, im / rhs};
    }

    constexpr bool operator>(const cmplx_t& rhs) const noexcept {
        return abs2() > rhs.abs2();
    }

    constexpr bool operator<(const cmplx_t& rhs) const noexcept {
        return abs2() < rhs.abs2();
    }

    constexpr bool operator==(const cmplx_t& rhs) const noexcept {
        return (re == rhs.re) && (im == rhs.im);
    }

    constexpr bool operator!=(const cmplx_t& rhs) const noexcept {
        return !(*this == rhs);
    }

    [[nodiscard]] constexpr cmplx_t conj() const noexcept {
        return {re, -im};
    }

    [[nodiscard]] constexpr real_t abs2() const noexcept {
        return re * re + im * im;
    }

    friend std::ostream& operator<<(std::ostream& os, const cmplx_t& x);
};

//left oriented real * cmplx
template<class T, class S_ = typename enable_scalar_t<T>::type,
         class C_ = typename enable_convertible_t<T, cmplx_t>::type>
constexpr cmplx_t operator+(const T& lhs, const cmplx_t& rhs) {
    return rhs + lhs;
}

template<class T, class S_ = typename enable_scalar_t<T>::type,
         class C_ = typename enable_convertible_t<T, cmplx_t>::type>
constexpr cmplx_t operator-(const T& lhs, const cmplx_t& rhs) {
    return {lhs - rhs.re, -rhs.im};
}

template<class T, class S_ = typename enable_scalar_t<T>::type,
         class C_ = typename enable_convertible_t<T, cmplx_t>::type>
constexpr cmplx_t operator*(const T& lhs, const cmplx_t& rhs) {
    return rhs * lhs;
}

template<class T, class S_ = typename enable_scalar_t<T>::type,
         class C_ = typename enable_convertible_t<T, cmplx_t>::type>
constexpr cmplx_t operator/(const T& lhs, const cmplx_t& rhs) {
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