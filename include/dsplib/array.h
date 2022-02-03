#pragma once

#include <vector>
#include <dsplib/types.h>
#include <dsplib/slice.h>
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

template<typename T>
class base_array
{
public:
    base_array()
    {}

    explicit base_array(int n)
    {
        _vec.resize(n);
        std::fill(_vec.data(), _vec.data() + n, 0);
    }

    base_array(const slice<T>& slice)
    {
        _vec.resize((slice.p2 - slice.p1) / slice.step);
        auto r = this->slice(0, _vec.size());
        r = slice;
    }

    base_array<T>& operator=(const slice<T>& rhs)
    {
        *this = base_array<T>(rhs);
        return *this;
    }

    explicit base_array(const std::vector<T>& v)
      : _vec(v)
    {}

    template<typename T2>
    explicit base_array(const std::vector<T2>& v)
    {
        static_assert(std::is_convertible<T2, T>::value, "Type is not convertible");
        _vec.assign(v.begin(), v.end());
    }

    base_array(std::vector<T>&& v)
      : _vec(std::move(v))
    {}

    base_array(const base_array<T>& v)
      : _vec(v._vec)
    {}

    template<class T2>
    base_array(const base_array<T2>& v)
    {
        static_assert(std::is_convertible<T2, T>::value, "Type is not convertible");
        _vec.assign(v.begin(), v.end());
    }

    base_array(base_array<T>&& v)
      : _vec(std::move(v._vec))
    {}

    base_array(const std::initializer_list<T>& list)
    {
        _vec = std::vector<T>(list);
    }

    template<typename T2>
    explicit base_array(const T2* x, size_t nx)
    {
        static_assert(std::is_convertible<T2, T>::value, "Type is not convertible");
        _vec.insert(_vec.end(), x, x + nx);
    }

    //--------------------------------------------------------------------
    base_array<T>& operator=(const base_array<T>& rhs)
    {
        if (this == &rhs) {
            return *this;
        }

        if (this->size() != rhs.size()) {
            _vec.resize(rhs.size());
        }

        std::copy(rhs.data(), (rhs.data() + rhs.size()), _vec.data());
        return *this;
    }

    base_array<T>& operator=(base_array<T>&& rhs)
    {
        if (this == &rhs) {
            return *this;
        }

        _vec.swap(rhs._vec);
        return *this;
    }

    //--------------------------------------------------------------------
    const T& operator[](int i) const
    {
        if (i >= 0) {
            return _vec[i];
        } else {
            return _vec[_vec.size() + i];
        }
    }

    T& operator[](int i)
    {
        if (i >= 0) {
            return _vec[i];
        } else {
            return _vec[_vec.size() + i];
        }
    }

    //--------------------------------------------------------------------
    typedef typename std::vector<T>::iterator iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;

    iterator begin()
    {
        return _vec.begin();
    }

    iterator end()
    {
        return _vec.end();
    }

    const_iterator begin() const
    {
        return _vec.begin();
    }

    const_iterator end() const
    {
        return _vec.end();
    }

    void push_back(const T& v)
    {
        _vec.push_back(v);
    }

    void push_front(const T& v)
    {
        _vec.insert(_vec.begin(), v);
    }

    T pop_back()
    {
        auto r = _vec.back();
        _vec.pop_back();
        return r;
    }

    T pop_front()
    {
        auto r = _vec.front();
        memmove(_vec.data(), _vec.data() + 1, (_vec.size() - 1) * sizeof(T));
        _vec.resize(_vec.size() - 1);
        return r;
    }

    T* data()
    {
        return _vec.data();
    }

    const T* data() const
    {
        return _vec.data();
    }

    int size() const
    {
        return _vec.size();
    }

    bool empty() const
    {
        return _vec.empty();
    }

    //--------------------------------------------------------------------
    base_array<T>& operator+()
    {
        return *this;
    }

    base_array<T>& operator-()
    {   
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] = (-_vec[i]);
        }
        return *this;
    }

    //--------------------------------------------------------------------
    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator+=(const T2& rhs)
    {
        static_assert(std::is_same<T, R>::value, "The operation changes the type");
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] += rhs;
        }
        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator-=(const T2& rhs)
    {
        static_assert(std::is_same<T, R>::value, "The operation changes the type");
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] -= rhs;
        }
        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator*=(const T2& rhs)
    {
        static_assert(std::is_same<T, R>::value, "The operation changes the type");
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] *= rhs;
        }
        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator/=(const T2& rhs)
    {
        static_assert(std::is_same<T, R>::value, "The operation changes the type");
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] /= rhs;
        }
        return *this;
    }

    //--------------------------------------------------------------------
    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator+(const T2& rhs) const
    {
        base_array<R> temp(*this);
        temp += rhs;
        return temp;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator-(const T2& rhs) const
    {
        base_array<R> temp(*this);
        temp -= rhs;
        return temp;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator*(const T2& rhs) const
    {
        base_array<R> temp(*this);
        temp *= rhs;
        return temp;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator/(const T2& rhs) const
    {
        base_array<R> temp(*this);
        temp /= rhs;
        return temp;
    }

    //--------------------------------------------------------------------
    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator+=(const base_array<T2>& rhs)
    {
        if (this->size() != rhs.size()) {
            throw std::invalid_argument("array sizes are different");
        }

        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] += rhs[i];
        }

        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator-=(const base_array<T2>& rhs)
    {
        if (this->size() != rhs.size()) {
            throw std::invalid_argument("array sizes are different");
        }

        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] -= rhs[i];
        }

        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator*=(const base_array<T2>& rhs)
    {
        if (this->size() != rhs.size()) {
            throw std::invalid_argument("array sizes are different");
        }

        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] *= rhs[i];
        }

        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator/=(const base_array<T2>& rhs)
    {
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
    base_array<R> operator+(const base_array<T2>& rhs) const
    {
        base_array<R> temp(*this);
        temp += rhs;
        return temp;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator-(const base_array<T2>& rhs) const
    {
        base_array<R> temp(*this);
        temp -= rhs;
        return temp;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator*(const base_array<T2>& rhs) const
    {
        base_array<R> temp(*this);
        temp *= rhs;
        return temp;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator/(const base_array<T2>& rhs) const
    {
        base_array<R> temp(*this);
        temp /= rhs;
        return temp;
    }

    //--------------------------------------------------------------------
    base_array<T>& operator^=(const real_t& rhs)
    {
        for (size_t i = 0; i < _vec.size(); ++i) {
            _vec[i] = pow(_vec[i], rhs);
        }
        return *this;
    }

    base_array<T> operator^(const real_t& rhs) const
    {
        base_array<T> temp(*this);
        temp ^= rhs;
        return temp;
    }

    //--------------------------------------------------------------------
    //concatenate syntax
    template<class T2, class R = ResultType<T, T2>>
    base_array<R>& operator|=(const base_array<T2>& rhs)
    {
        _vec.insert(_vec.end(), rhs.begin(), rhs.end());
        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_array<R> operator|(const base_array<T2>& rhs) const
    {
        base_array<R> temp(*this);
        temp |= rhs;
        return temp;
    }

    //--------------------------------------------------------------------
    ::dsplib::slice<T> slice(int i1, int i2, int m = 1)
    {
        if (m < 1) {
            throw std::range_error("Step must be greater 1");
        }

        if (i2 - i1 > _vec.size()) {
            throw std::range_error("Slice range is greater vector size");
        }

        ::dsplib::slice<T> r;
        r.p1 = i1;
        r.p2 = i2;
        r.step = m;
        r.data = _vec.data();
        r.size = _vec.size();
        return r;
    }

    template<typename R>
    std::vector<R> to_vec() const
    {
        static_assert(std::is_convertible<T, R>::value, "Type is not convertible");
        return std::vector<R>(_vec.begin(), _vec.end());
    }

protected:
    std::vector<T> _vec;
};

//--------------------------------------------------------------------------------
template<class T>
inline base_array<T> operator+(const real_t& lhs, const base_array<T>& rhs)
{
    return rhs + lhs;
}

template<class T>
inline base_array<T> operator-(const real_t& lhs, const base_array<T>& rhs)
{
    base_array<T> r(rhs);
    return (-r) + lhs;
}

template<class T>
inline base_array<T> operator*(const real_t& lhs, const base_array<T>& rhs)
{
    return rhs * lhs;
}

template<class T>
inline base_array<T> operator/(const real_t& lhs, const base_array<T>& rhs)
{
    base_array<T> r(rhs);
    for (size_t i = 0; i < r.size(); ++i) {
        r[i] = lhs / rhs[i];
    }

    return r;
}

//----------------------------------------------------------------------------------------
inline base_array<cmplx_t> operator*(const base_array<real_t>& lhs, const std::complex<double>& rhs)
{
    return lhs * cmplx_t(rhs);
}

inline base_array<cmplx_t> operator*(const std::complex<double>& lhs, const base_array<real_t>& rhs)
{
    return rhs * cmplx_t(lhs);
}

//----------------------------------------------------------------------------------------
using slice_real = slice<real_t>;
using slice_cmplx = slice<cmplx_t>;

using arr_real = base_array<real_t>;
using arr_cmplx = base_array<cmplx_t>;

}   // namespace dsplib
