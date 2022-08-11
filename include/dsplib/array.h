#pragma once

#include <vector>
#include <cassert>
#include <cmath>

#include <dsplib/types.h>
#include <dsplib/slice.h>
#include <dsplib/allocator.h>
#include <dsplib/indexing.h>
#include <memory.h>

namespace dsplib {

template<bool _Cond, typename _Iftrue, typename _Iffalse>
using conditional_t = typename std::conditional<_Cond, _Iftrue, _Iffalse>::type;

//Detect out type for operations:
//real + real -> real
//cmplx + cmplx -> cmplx
//real + cmplx -> cmplx
//cmplx + real -> cmplx
template<typename Bt, typename It>
using ResultType = conditional_t<std::is_same<Bt, cmplx_t>::value || std::is_same<It, cmplx_t>::value, cmplx_t, real_t>;

//TODO: add array_view as parent for array/slice
template<typename T>
class base_array
{
public:
    base_array() = default;

    explicit base_array(int n)
      : _vec(n) {
    }

    base_array(const const_slice_t<T>& rhs) {
        _vec.resize(rhs.size());
        this->slice(0, indexing::end) = rhs;
    }

    base_array<T>& operator=(const const_slice_t<T>& rhs) {
        *this = base_array<T>(rhs);
        return *this;
    }

    base_array(const slice_t<T>& rhs)
      : base_array(const_slice_t<T>(rhs)) {
    }

    base_array<T>& operator=(const slice_t<T>& rhs) {
        *this = base_array<T>(const_slice_t<T>(rhs));
        return *this;
    }

    //TODO: remove explicit?
    explicit base_array(const std::vector<T>& v)
      : _vec(v.begin(), v.end()) {
    }

    template<typename T2>
    explicit base_array(const std::vector<T2>& v)
      : _vec{v.begin(), v.end()} {
        static_assert(std::is_convertible<T2, T>::value, "Type is not convertible");
    }

    base_array(std::vector<T>&& v)
      : _vec(std::move(v)) {
    }

    base_array(const base_array<T>& v)
      : _vec(v._vec) {
    }

    template<class T2>
    base_array(const base_array<T2>& v)
      : _vec(v.size()) {
        static_assert(std::is_convertible<T2, T>::value, "Type is not convertible");
        _vec.assign(v.begin(), v.end());
    }

    base_array(base_array<T>&& v)
      : _vec(std::move(v._vec)) {
    }

    base_array(const std::initializer_list<T>& v)
      : _vec{v.begin(), v.end()} {
    }

    template<typename T2>
    explicit base_array(const T2* x, size_t nx)
      : _vec(nx) {
        static_assert(std::is_convertible<T2, T>::value, "Type is not convertible");
        _vec.assign(x, x + nx);
    }

    //--------------------------------------------------------------------
    base_array<T>& operator=(const base_array<T>& rhs) {
        if (this == &rhs) {
            return *this;
        }

        if (this->size() != rhs.size()) {
            _vec.resize(rhs.size());
        }

        std::copy(rhs.data(), (rhs.data() + rhs.size()), _vec.data());
        return *this;
    }

    base_array<T>& operator=(base_array<T>&& rhs) {
        if (this == &rhs) {
            return *this;
        }

        _vec.swap(rhs._vec);
        return *this;
    }

    //--------------------------------------------------------------------
    const T& operator[](int i) const noexcept {
        const int idx = (i >= 0) ? (i) : (_vec.size() + i);
        assert((idx >= 0) && (idx < _vec.size()));
        return _vec[idx];
    }

    T& operator[](int i) noexcept {
        const int idx = (i >= 0) ? (i) : (_vec.size() + i);
        assert((idx >= 0) && (idx < _vec.size()));
        return _vec[idx];
    }

    //--------------------------------------------------------------------
    const T& operator()(int i) const noexcept {
        return this->operator[](i);
    }

    T& operator()(int i) noexcept {
        return this->operator[](i);
    }

    //--------------------------------------------------------------------
    slice_t<T> slice(int i1, int i2, int m = 1) {
        return slice_t<T>(*this, i1, i2, m);
    }

    const_slice_t<T> slice(int i1, int i2, int m = 1) const {
        return const_slice_t<T>(*this, i1, i2, m);
    }

    slice_t<T> slice(int i1, indexing::end_t, int m = 1) {
        return slice_t<T>(*this, i1, size(), m);
    }

    const_slice_t<T> slice(int i1, indexing::end_t, int m = 1) const {
        return const_slice_t<T>(*this, i1, size(), m);
    }

    //--------------------------------------------------------------------
    typedef typename std::vector<T>::iterator iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;

    iterator begin() noexcept {
        return _vec.begin();
    }

    iterator end() noexcept {
        return _vec.end();
    }

    const_iterator begin() const noexcept {
        return _vec.begin();
    }

    const_iterator end() const noexcept {
        return _vec.end();
    }

    T* data() {
        return _vec.data();
    }

    const T* data() const noexcept {
        return _vec.data();
    }

    int size() const noexcept {
        return _vec.size();
    }

    bool empty() const noexcept {
        return _vec.empty();
    }

    //--------------------------------------------------------------------
    base_array<T>& operator+() noexcept {
        return *this;
    }

    base_array<T>& operator-() noexcept {
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] = (-_vec[i]);
        }
        return *this;
    }

    //--------------------------------------------------------------------
    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator+=(const T2& rhs) noexcept {
        static_assert(std::is_same<T, R>::value, "The operation changes the type");
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] += rhs;
        }
        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator-=(const T2& rhs) noexcept {
        static_assert(std::is_same<T, R>::value, "The operation changes the type");
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] -= rhs;
        }
        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator*=(const T2& rhs) noexcept {
        static_assert(std::is_same<T, R>::value, "The operation changes the type");
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] *= rhs;
        }
        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator/=(const T2& rhs) {
        static_assert(std::is_same<T, R>::value, "The operation changes the type");
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] /= rhs;
        }
        return *this;
    }

    //--------------------------------------------------------------------
    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator+(const T2& rhs) const {
        base_array<R> temp(*this);
        temp += rhs;
        return temp;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator-(const T2& rhs) const {
        base_array<R> temp(*this);
        temp -= rhs;
        return temp;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator*(const T2& rhs) const {
        base_array<R> temp(*this);
        temp *= rhs;
        return temp;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator/(const T2& rhs) const {
        base_array<R> temp(*this);
        temp /= rhs;
        return temp;
    }

    //--------------------------------------------------------------------
    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator+=(const base_array<T2>& rhs) {
        if (this->size() != rhs.size()) {
            throw std::invalid_argument("array sizes are different");
        }

        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] += rhs[i];
        }

        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator-=(const base_array<T2>& rhs) {
        if (this->size() != rhs.size()) {
            throw std::invalid_argument("array sizes are different");
        }

        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] -= rhs[i];
        }

        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator*=(const base_array<T2>& rhs) {
        if (this->size() != rhs.size()) {
            throw std::invalid_argument("array sizes are different");
        }

        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] *= rhs[i];
        }

        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator/=(const base_array<T2>& rhs) {
        if (this->size() != rhs.size()) {
            throw std::invalid_argument("array sizes are different");
        }

        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] /= rhs[i];
        }

        return *this;
    }

    //--------------------------------------------------------------------
    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator+(const base_array<T2>& rhs) const {
        base_array<R> temp(*this);
        temp += rhs;
        return temp;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator-(const base_array<T2>& rhs) const {
        base_array<R> temp(*this);
        temp -= rhs;
        return temp;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator*(const base_array<T2>& rhs) const {
        base_array<R> temp(*this);
        temp *= rhs;
        return temp;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator/(const base_array<T2>& rhs) const {
        base_array<R> temp(*this);
        temp /= rhs;
        return temp;
    }

    //--------------------------------------------------------------------
    //TODO: operation priority can be misleading
    base_array<T>& operator^=(const real_t& rhs) {
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] = std::pow(_vec[i], rhs);
        }
        return *this;
    }

    base_array<T> operator^(const real_t& rhs) const {
        base_array<T> temp(*this);
        temp ^= rhs;
        return temp;
    }

    //--------------------------------------------------------------------
    //TODO: implement pow(cmplx_t, cmplx_t)
    base_array<T>& operator^=(const base_array<real_t>& rhs) {
        if (rhs.size() != _vec.size()) {
            throw std::invalid_argument("different vector size");
        }
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] = std::pow(_vec[i], rhs[i]);
        }
        return *this;
    }

    base_array<T> operator^(const base_array<real_t>& rhs) const {
        base_array<T> temp(*this);
        temp ^= rhs;
        return temp;
    }

    //--------------------------------------------------------------------
    //concatenate syntax
    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator|=(const base_array<T2>& rhs) {
        _vec.insert(_vec.end(), rhs.begin(), rhs.end());
        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator|(const base_array<T2>& rhs) const {
        base_array<R> temp(*this);
        temp |= rhs;
        return temp;
    }

    template<typename R>
    std::vector<R> to_vec() const {
        static_assert(std::is_convertible<T, R>::value, "Type is not convertible");
        return std::vector<R>(_vec.begin(), _vec.end());
    }

protected:
    std::vector<T, allocator<T>> _vec;
};

//--------------------------------------------------------------------------------
//left oriented scalar * array
template<class T, class Scalar, class R = ResultType<T, Scalar>, class _S = typename enable_scalar_t<Scalar>::type,
         class _C = typename enable_convertible_t<Scalar, R>::type>
inline base_array<R> operator+(const Scalar& lhs, const base_array<T>& rhs) {
    return rhs + R(lhs);
}

template<class T, class Scalar, class R = ResultType<T, Scalar>, class _S = typename enable_scalar_t<Scalar>::type,
         class _C = typename enable_convertible_t<Scalar, R>::type>
inline base_array<R> operator-(const Scalar& lhs, const base_array<T>& rhs) {
    base_array<R> r(rhs);
    for (size_t i = 0; i < r.size(); ++i) {
        r[i] = R(lhs) - rhs[i];
    }
    return r;
}

template<class T, class Scalar, class R = ResultType<T, Scalar>, class _S = typename enable_scalar_t<Scalar>::type,
         class _C = typename enable_convertible_t<Scalar, R>::type>
inline base_array<R> operator*(const Scalar& lhs, const base_array<T>& rhs) {
    return rhs * R(lhs);
}

template<class T, class Scalar, class R = ResultType<T, Scalar>, class _S = typename enable_scalar_t<Scalar>::type,
         class _C = typename enable_convertible_t<Scalar, R>::type>
inline base_array<R> operator/(const Scalar& lhs, const base_array<T>& rhs) {
    base_array<R> r(rhs);
    for (size_t i = 0; i < r.size(); ++i) {
        r[i] = R(lhs) / rhs[i];
    }
    return r;
}

//----------------------------------------------------------------------------------------
inline base_array<cmplx_t> operator*(const base_array<real_t>& lhs, const std::complex<double>& rhs) {
    return lhs * cmplx_t(rhs);
}

inline base_array<cmplx_t> operator*(const std::complex<double>& lhs, const base_array<real_t>& rhs) {
    return rhs * cmplx_t(lhs);
}

//----------------------------------------------------------------------------------------
using arr_real = base_array<real_t>;
using arr_cmplx = base_array<cmplx_t>;
using arr_int = base_array<int>;

}   // namespace dsplib
