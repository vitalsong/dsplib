#pragma once

#include <vector>
#include <cassert>
#include <cmath>

#include <dsplib/types.h>
#include <dsplib/slice.h>
#include <dsplib/indexing.h>
#include <dsplib/assert.h>

namespace dsplib {

template<bool Cond_, typename Iftrue_, typename Iffalse_>
using conditional_t = typename std::conditional_t<Cond_, Iftrue_, Iffalse_>;

//Detect out type for operations:
//real + real -> real
//cmplx + cmplx -> cmplx
//real + cmplx -> cmplx
//cmplx + real -> cmplx
template<typename T1, typename T2>
using ResultType = conditional_t<std::is_same_v<T1, cmplx_t> || std::is_same_v<T2, cmplx_t>, cmplx_t, real_t>;

template<typename T_dst, typename T_src>
base_array<T_dst> array_cast(const base_array<T_src>& src) noexcept {
    static_assert(is_scalar_v<T_src> && is_scalar_v<T_dst>, "types must be scalar");
    static_assert(!(is_complex_v<T_src> && !is_complex_v<T_dst>), "complex to real cast is not allowed");
    if constexpr (std::is_same_v<T_src, T_dst>) {
        return src;
    } else if constexpr (!is_complex_v<T_src> && is_complex_v<T_dst>) {
        base_array<T_dst> dst(src.size());
        for (int i = 0; i < src.size(); ++i) {
            dst[i].re = static_cast<real_t>(src[i]);
        }
        return dst;
    } else {
        return base_array<T_dst>(src);
    }
}

//rules for implicit array conversion
//TODO: use static_assert and verbose error message
template<typename T_src, typename T_dst>
constexpr bool is_array_convertible() noexcept {
    if constexpr (!std::is_convertible_v<T_src, T_dst>) {
        return false;
    }

    //only arithmetic scalar
    if constexpr (!is_scalar_v<T_src> || !is_scalar_v<T_dst>) {
        return false;
    }

    //cmplx -> real
    if constexpr (is_complex_v<T_src> && !is_complex_v<T_dst>) {
        return false;
    }

    //real -> cmplx
    if constexpr (!is_complex_v<T_src> && is_complex_v<T_dst>) {
        return false;
    }

    //float -> int
    if constexpr (std::is_floating_point_v<T_src> && std::is_integral_v<T_dst>) {
        return false;
    }

    return true;
}

//base dsplib array type
//TODO: add array_view as parent for array/slice
//TODO: add slice(vector<bool>)
template<typename T>
class base_array
{
public:
    base_array() = default;

    explicit base_array(int n)
      : _vec(n, 0) {
    }

    base_array(const const_slice_t<T>& rhs)
      : base_array(rhs.size()) {
        this->slice(0, indexing::end) = rhs;
    }

    base_array(const slice_t<T>& rhs)
      : base_array(const_slice_t<T>(rhs)) {
    }

    base_array(const std::vector<T>& v)
      : _vec(v) {
    }

    template<typename T2>
    base_array(const std::vector<T2>& v) {
        static_assert(is_array_convertible<T2, T>(), "only real->real, cmplx->cmplx array cast support");
        _vec.assign(v.begin(), v.end());
    }

    base_array(std::vector<T>&& v)
      : _vec(std::move(v)) {
    }

    base_array(const base_array<T>& v)
      : _vec(v._vec) {
    }

    template<typename T2>
    base_array(const base_array<T2>& v) {
        static_assert(is_array_convertible<T2, T>(), "only real->real, cmplx->cmplx array cast support");
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
        static_assert(is_array_convertible<T2, T>(), "only real->real, cmplx->cmplx array cast support");
        _vec.insert(_vec.end(), x, x + nx);
    }

    //--------------------------------------------------------------------
    base_array<T>& operator=(const base_array<T>& rhs) {
        if (this == &rhs) {
            return *this;
        }
        _vec = rhs._vec;
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
        assert((idx >= 0) && (idx < int(_vec.size())));
        return _vec[idx];
    }

    T& operator[](int i) noexcept {
        const int idx = (i >= 0) ? (i) : (_vec.size() + i);
        assert((idx >= 0) && (idx < int(_vec.size())));
        return _vec[idx];
    }

    //--------------------------------------------------------------------
    const T& operator[](size_t i) const noexcept {
        assert(i < _vec.size());
        return _vec[i];
    }

    T& operator[](size_t i) noexcept {
        assert(i < _vec.size());
        return _vec[i];
    }

    //--------------------------------------------------------------------
    base_array<T> operator[](const std::vector<bool>& idxs) const {
        DSPLIB_ASSERT(idxs.size() == _vec.size(), "array sizes must be equal");
        std::vector<T> res;
        res.reserve(_vec.size());
        for (size_t i = 0; i < idxs.size(); ++i) {
            if (idxs[i]) {
                res.push_back(_vec[i]);
            }
        }
        return res;
    }

    base_array<T> operator[](const std::vector<int>& idxs) const {
        const size_t max_i = *std::max_element(idxs.begin(), idxs.end());
        DSPLIB_ASSERT(max_i < _vec.size(), "index must not exceed the size of the vector");
        std::vector<T> res(idxs.size());
        for (size_t i = 0; i < idxs.size(); ++i) {
            res[i] = _vec[idxs[i]];
        }
        return res;
    }

    base_array<T> operator[](const base_array<int>& idxs) const {
        return (*this)[idxs.to_vec()];
    }

    //--------------------------------------------------------------------
    //compare scalar
    std::vector<bool> operator>(T val) const noexcept {
        std::vector<bool> res(_vec.size());
        for (size_t i = 0; i < _vec.size(); ++i) {
            res[i] = (_vec[i] > val);
        }
        return res;
    }

    std::vector<bool> operator<(T val) const noexcept {
        std::vector<bool> res(_vec.size());
        for (size_t i = 0; i < _vec.size(); ++i) {
            res[i] = (_vec[i] < val);
        }
        return res;
    }

    std::vector<bool> operator==(T val) const noexcept {
        std::vector<bool> res(_vec.size());
        for (size_t i = 0; i < _vec.size(); ++i) {
            //TODO: add tolerance for compare?
            res[i] = (_vec[i] == val);
        }
        return res;
    }

    std::vector<bool> operator!=(T val) const noexcept {
        auto r = (*this == val);
        r.flip();
        return r;
    }

    //--------------------------------------------------------------------
    //compare vector
    std::vector<bool> operator>(const base_array<T>& rhs) const noexcept {
        std::vector<bool> res(_vec.size());
        for (size_t i = 0; i < _vec.size(); ++i) {
            res[i] = (_vec[i] > rhs._vec[i]);
        }
        return res;
    }

    std::vector<bool> operator<(const base_array<T>& rhs) const noexcept {
        std::vector<bool> res(_vec.size());
        for (size_t i = 0; i < _vec.size(); ++i) {
            res[i] = (_vec[i] < rhs._vec[i]);
        }
        return res;
    }

    std::vector<bool> operator==(const base_array<T>& rhs) const noexcept {
        std::vector<bool> res(_vec.size());
        for (size_t i = 0; i < _vec.size(); ++i) {
            res[i] = (_vec[i] == rhs._vec[i]);
        }
        return res;
    }

    std::vector<bool> operator!=(const base_array<T>& rhs) const noexcept {
        auto r = (*this == rhs);
        r.flip();
        return r;
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

    //TODO: add slice(end, first, -1) like x[::-1] numpy
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

    T* data() noexcept {
        return _vec.data();
    }

    const T* data() const noexcept {
        return _vec.data();
    }

    [[nodiscard]] int size() const noexcept {
        return int(_vec.size());
    }

    [[nodiscard]] bool empty() const noexcept {
        return _vec.empty();
    }

    //--------------------------------------------------------------------
    const base_array<T>& operator+() const noexcept {
        return *this;
    }

    base_array<T> operator-() const noexcept {
        base_array<T> r{_vec};
        for (int i = 0; i < r.size(); ++i) {
            r[i] = -r[i];
        }
        return r;
    }

    //--------------------------------------------------------------------
    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator+=(const T2& rhs) noexcept {
        static_assert(std::is_same_v<T, R>, "the operation changes the type");
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] += rhs;
        }
        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator-=(const T2& rhs) noexcept {
        static_assert(std::is_same_v<T, R>, "the operation changes the type");
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] -= rhs;
        }
        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator*=(const T2& rhs) noexcept {
        static_assert(std::is_same_v<T, R>, "the operation changes the type");
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] *= rhs;
        }
        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator/=(const T2& rhs) noexcept {
        static_assert(std::is_same_v<T, R>, "the operation changes the type");
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] /= rhs;
        }
        return *this;
    }

    //--------------------------------------------------------------------
    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator+(const T2& rhs) const {
        auto temp = array_cast<R>(*this);
        //TODO: optimize, not optimal for arr_cmplx + real_t
        temp += rhs;
        return temp;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator-(const T2& rhs) const {
        auto temp = array_cast<R>(*this);
        temp -= rhs;
        return temp;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator*(const T2& rhs) const {
        auto temp = array_cast<R>(*this);
        temp *= rhs;
        return temp;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator/(const T2& rhs) const {
        auto temp = array_cast<R>(*this);
        temp /= rhs;
        return temp;
    }

    //--------------------------------------------------------------------
    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator+=(const base_array<T2>& rhs) {
        DSPLIB_ASSERT(this->size() == rhs.size(), "arrays sizes must be equal");
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] += rhs[i];
        }
        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator-=(const base_array<T2>& rhs) {
        DSPLIB_ASSERT(this->size() == rhs.size(), "arrays sizes must be equal");
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] -= rhs[i];
        }
        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator*=(const base_array<T2>& rhs) {
        DSPLIB_ASSERT(this->size() == rhs.size(), "arrays sizes must be equal");
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] *= rhs[i];
        }
        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator/=(const base_array<T2>& rhs) {
        DSPLIB_ASSERT(this->size() == rhs.size(), "arrays sizes must be equal");
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] /= rhs[i];
        }
        return *this;
    }

    //--------------------------------------------------------------------
    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator+(const base_array<T2>& rhs) const {
        auto temp = array_cast<R>(*this);
        temp += rhs;
        return temp;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator-(const base_array<T2>& rhs) const {
        auto temp = array_cast<R>(*this);
        temp -= rhs;
        return temp;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator*(const base_array<T2>& rhs) const {
        auto temp = array_cast<R>(*this);
        temp *= rhs;
        return temp;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator/(const base_array<T2>& rhs) const {
        auto temp = array_cast<R>(*this);
        temp /= rhs;
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
        auto temp = array_cast<R>(*this);
        temp |= rhs;
        return temp;
    }

    template<typename R>
    std::vector<R> to_vec() const {
        if constexpr (std::is_same_v<T, R>) {
            return _vec;
        } else {
            static_assert(std::is_convertible_v<T, R>, "type is not convertible");
            return std::vector<R>(_vec.begin(), _vec.end());
        }
    }

    const std::vector<T>& to_vec() const noexcept {
        return _vec;
    }

    //apply per element function
    template<class Fn>
    auto apply(Fn func) const {
        using R = decltype(func(T()));
        if constexpr (std::is_same_v<T, R>) {
            base_array<T> out(*this);
            for (T& v : out) {
                v = func(v);
            }
            return out;
        } else {
            base_array<R> out(size());
            R* pout = out.data();
            for (const T& v : _vec) {
                *pout++ = func(v);
            }
            return out;
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const base_array& x) {
        return x._print(os);
    }

protected:
    std::ostream& _print(std::ostream& os) const;
    std::vector<T> _vec;
};

//--------------------------------------------------------------------------------
//left oriented scalar * array
template<class T, class Scalar, class R = ResultType<T, Scalar>, class S_ = enable_scalar_t<Scalar>,
         class C_ = enable_convertible_t<Scalar, R>>
inline base_array<R> operator+(const Scalar& lhs, const base_array<T>& rhs) {
    return rhs + R(lhs);
}

template<class T, class Scalar, class R = ResultType<T, Scalar>, class S_ = enable_scalar_t<Scalar>,
         class C_ = enable_convertible_t<Scalar, R>>
inline base_array<R> operator-(const Scalar& lhs, const base_array<T>& rhs) {
    auto r = array_cast<R>(rhs);
    for (int i = 0; i < r.size(); ++i) {
        r[i] = R(lhs) - rhs[i];
    }
    return r;
}

template<class T, class Scalar, class R = ResultType<T, Scalar>, class S_ = enable_scalar_t<Scalar>,
         class C_ = enable_convertible_t<Scalar, R>>
inline base_array<R> operator*(const Scalar& lhs, const base_array<T>& rhs) {
    return rhs * R(lhs);
}

template<class T, class Scalar, class R = ResultType<T, Scalar>, class S_ = enable_scalar_t<Scalar>,
         class C_ = enable_convertible_t<Scalar, R>>
inline base_array<R> operator/(const Scalar& lhs, const base_array<T>& rhs) {
    auto r = array_cast<R>(rhs);
    for (int i = 0; i < r.size(); ++i) {
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
