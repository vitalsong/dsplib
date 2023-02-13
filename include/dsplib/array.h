#pragma once

#include <vector>
#include <cassert>
#include <cmath>

#include <dsplib/types.h>
#include <dsplib/slice.h>
#include <dsplib/indexing.h>
#include <dsplib/throw.h>

namespace dsplib {

template<bool Cond_, typename Iftrue_, typename Iffalse_>
using conditional_t = typename std::conditional_t<Cond_, Iftrue_, Iffalse_>;

//Detect out type for operations:
//real + real -> real
//cmplx + cmplx -> cmplx
//real + cmplx -> cmplx
//cmplx + real -> cmplx
template<typename Bt, typename It>
using ResultType = conditional_t<std::is_same_v<Bt, cmplx_t> || std::is_same_v<It, cmplx_t>, cmplx_t, real_t>;

//TODO: add array_view as parent for array/slice
template<typename T>
class base_array
{
public:
    base_array() = default;

    explicit base_array(int n) {
        _vec.resize(n);
        std::fill(_vec.data(), _vec.data() + n, 0);
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

    base_array(const std::vector<T>& v)
      : _vec(v) {
    }

    template<typename T2>
    base_array(const std::vector<T2>& v) {
        static_assert(std::is_convertible<T2, T>::value, "Type is not convertible");
        _vec.assign(v.begin(), v.end());
    }

    base_array(std::vector<T>&& v)
      : _vec(std::move(v)) {
    }

    base_array(const base_array<T>& v)
      : _vec(v._vec) {
    }

    template<class T2>
    base_array(const base_array<T2>& v) {
        static_assert(std::is_convertible<T2, T>::value, "Type is not convertible");
        _vec.assign(v.begin(), v.end());
    }

    base_array(base_array<T>&& v) noexcept
      : _vec(std::move(v._vec)) {
    }

    base_array(const std::initializer_list<T>& list)
      : _vec(list) {
    }

    template<typename T2>
    explicit base_array(const T2* x, size_t nx) {
        static_assert(std::is_convertible<T2, T>::value, "Type is not convertible");
        _vec.insert(_vec.end(), x, x + nx);
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

    base_array<T>& operator=(base_array<T>&& rhs) noexcept {
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
    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;

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

    void push_back(const T& v) {
        _vec.push_back(v);
    }

    void push_front(const T& v) {
        _vec.insert(_vec.begin(), v);
    }

    T pop_back() {
        auto r = _vec.back();
        _vec.pop_back();
        return r;
    }

    T pop_front() {
        auto r = _vec.front();
        std::memmove(_vec.data(), _vec.data() + 1, (_vec.size() - 1) * sizeof(T));
        _vec.resize(_vec.size() - 1);
        return r;
    }

    T* data() noexcept {
        return _vec.data();
    }

    const T* data() const noexcept {
        return _vec.data();
    }

    [[nodiscard]] int size() const noexcept {
        return _vec.size();
    }

    [[nodiscard]] bool empty() const noexcept {
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
        static_assert(std::is_same_v<T, R>, "The operation changes the type");
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] += rhs;
        }
        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator-=(const T2& rhs) noexcept {
        static_assert(std::is_same_v<T, R>, "The operation changes the type");
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] -= rhs;
        }
        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator*=(const T2& rhs) noexcept {
        static_assert(std::is_same_v<T, R>, "The operation changes the type");
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] *= rhs;
        }
        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator/=(const T2& rhs) {
        static_assert(std::is_same_v<T, R>, "The operation changes the type");
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
            DSPLIB_THROW("array sizes are different");
        }

        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] += rhs[i];
        }

        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator-=(const base_array<T2>& rhs) {
        if (this->size() != rhs.size()) {
            DSPLIB_THROW("array sizes are different");
        }

        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] -= rhs[i];
        }

        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator*=(const base_array<T2>& rhs) {
        if (this->size() != rhs.size()) {
            DSPLIB_THROW("array sizes are different");
        }

        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] *= rhs[i];
        }

        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator/=(const base_array<T2>& rhs) {
        if (this->size() != rhs.size()) {
            DSPLIB_THROW("array sizes are different");
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
            DSPLIB_THROW("different vector size");
        }
        for (int i = 0; i < _vec.size(); ++i) {
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
        if constexpr (std::is_same_v<T, R>) {
            return _vec;
        } else {
            static_assert(std::is_convertible<T, R>::value, "Type is not convertible");
            return std::vector<R>(_vec.begin(), _vec.end());
        }
    }

    const std::vector<T>& to_vec() const noexcept {
        return _vec;
    }

    friend std::ostream& operator<<(std::ostream& os, const base_array<T>& x);

protected:
    std::vector<T> _vec;
};

//--------------------------------------------------------------------------------
//left oriented scalar * array
template<class T, class Scalar, class R = ResultType<T, Scalar>, class S_ = typename enable_scalar_t<Scalar>::type,
         class C_ = typename enable_convertible_t<Scalar, R>::type>
inline base_array<R> operator+(const Scalar& lhs, const base_array<T>& rhs) {
    return rhs + R(lhs);
}

template<class T, class Scalar, class R = ResultType<T, Scalar>, class S_ = typename enable_scalar_t<Scalar>::type,
         class C_ = typename enable_convertible_t<Scalar, R>::type>
inline base_array<R> operator-(const Scalar& lhs, const base_array<T>& rhs) {
    base_array<R> r(rhs);
    for (size_t i = 0; i < r.size(); ++i) {
        r[i] = R(lhs) - rhs[i];
    }
    return r;
}

template<class T, class Scalar, class R = ResultType<T, Scalar>, class S_ = typename enable_scalar_t<Scalar>::type,
         class C_ = typename enable_convertible_t<Scalar, R>::type>
inline base_array<R> operator*(const Scalar& lhs, const base_array<T>& rhs) {
    return rhs * R(lhs);
}

template<class T, class Scalar, class R = ResultType<T, Scalar>, class S_ = typename enable_scalar_t<Scalar>::type,
         class C_ = typename enable_convertible_t<Scalar, R>::type>
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
