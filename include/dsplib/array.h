#pragma once

#include <dsplib/types.h>
#include <vector>
#include <string.h>

namespace dsplib {

class arr_real;
class arr_cmplx;

class slice_real
{
public:
    slice_real() = default;
    slice_real(const slice_real &rhs) {
        *this = rhs;
    }

    slice_real& operator = (const slice_real &rhs);
    slice_real& operator = (const arr_real &rhs);
    slice_real& operator = (const real_t &value);
    slice_real& operator = (const std::initializer_list<real_t> &list);

    int p1 = 0;
    int p2 = 0;
    int step = 0;
    int size = 0;
    real_t* data = nullptr;
};

class slice_cmplx
{
public:
    slice_cmplx() = default;
    slice_cmplx(const slice_cmplx &rhs) {
        *this = rhs;
    }

    slice_cmplx& operator = (const slice_cmplx &rhs);
    slice_cmplx& operator = (const arr_cmplx &rhs);
    slice_cmplx& operator = (const cmplx_t &value);
    slice_cmplx& operator = (const std::initializer_list<cmplx_t> &list);

    int p1 = 0;
    int p2 = 0;
    int step = 0;
    int size = 0;
    cmplx_t* data = nullptr;
};

/*!
 * \brief Real array
 */
class arr_real
{
public:

    arr_real();
    arr_real(const arr_real& v);
    arr_real(arr_real&& v);
    arr_real(const slice_real& slice);

    arr_real(const std::initializer_list<real_t> &list) {
        _vec = std::vector<real_t>(list);
    }

    template <typename T>
    explicit arr_real(const T* x, size_t nx)
    {
        static_assert(std::is_convertible<T, real_t>::value, "Type is not convertible to dsplib::real_t");
        _vec.resize(nx);
        for (size_t i=0; i < nx; ++i) {
            _vec[i] = x[i];
        }
    }

    explicit arr_real(const std::vector<real_t>& v);
    explicit arr_real(int n);

    typedef std::vector<real_t>::iterator iterator;
    typedef std::vector<real_t>::const_iterator const_iterator;

    iterator begin() {
        return _vec.begin();
    }

    iterator end() {
        return _vec.end();
    }

    const_iterator begin() const {
        return _vec.begin();
    }

    const_iterator end() const {
        return _vec.end();
    }

    void push_back(const real_t& v) {
        _vec.push_back(v);
    }

    void push_front(const real_t& v) {
        _vec.insert(_vec.begin(), v);
    }

    real_t pop_back() {
        auto r = _vec.back();
        _vec.pop_back();
        return r;
    }

    real_t pop_front() {
        auto r = _vec.front();
        memmove(_vec.data(), _vec.data() + 1, (_vec.size() - 1) * sizeof(real_t));
        _vec.resize(_vec.size() - 1);
        return r;
    }

    real_t* data();
    const real_t* data() const;

    int size() const {
        return _vec.size();
    }

    arr_real& operator = (const arr_real &rhs);
    arr_real& operator = (arr_real &&rhs);

    arr_real& operator = (const slice_real &rhs) {
        *this = arr_real(rhs);
        return *this;
    }

    const real_t& operator [] (int i) const;
    real_t& operator [] (int i);

    arr_real& operator + ();
    arr_real& operator - ();

    arr_real& operator += (const arr_real& rhs);
    arr_real& operator -= (const arr_real& rhs);
    arr_real& operator *= (const arr_real& rhs);
    arr_real& operator /= (const arr_real& rhs);

    arr_real operator + (const arr_real& rhs) const;
    arr_real operator - (const arr_real& rhs) const;
    arr_real operator * (const arr_real& rhs) const;
    arr_real operator / (const arr_real& rhs) const;

    arr_cmplx operator + (const arr_cmplx& rhs) const;
    arr_cmplx operator - (const arr_cmplx& rhs) const;
    arr_cmplx operator * (const arr_cmplx& rhs) const;
    arr_cmplx operator / (const arr_cmplx& rhs) const;

    arr_real& operator += (const real_t& rhs);
    arr_real& operator -= (const real_t& rhs);
    arr_real& operator *= (const real_t& rhs);
    arr_real& operator /= (const real_t& rhs);

    arr_real operator + (const real_t& rhs) const;
    arr_real operator - (const real_t& rhs) const;
    arr_real operator * (const real_t& rhs) const;
    arr_real operator / (const real_t& rhs) const;

    arr_cmplx operator + (const cmplx_t& rhs) const;
    arr_cmplx operator - (const cmplx_t& rhs) const;
    arr_cmplx operator * (const cmplx_t& rhs) const;
    arr_cmplx operator / (const cmplx_t& rhs) const;

    //power
    arr_real& operator ^= (const real_t& base);
    arr_real operator ^ (const real_t& base) const;

    bool empty() const;

    slice_real slice(int i1, int i2, int m=1);

    template<typename T>
    std::vector<T> unpack() const {
        static_assert(std::is_convertible<real_t, T>::value, "Type is not convertible");
        static_assert(std::is_scalar<T>::value, "Type is not scalar");
        std::vector<T> res(_vec.size());
        for (size_t i = 0; i < _vec.size(); i++) {
            res[i] = _vec[i];
        }
        return res;
    }

private:
    std::vector <real_t> _vec;
};

/*!
 * \brief Complex array
 */
class arr_cmplx
{
public:
    arr_cmplx();
    arr_cmplx(const arr_cmplx& v);
    arr_cmplx(arr_cmplx&& v);
    arr_cmplx(const arr_real& v);
    arr_cmplx(const slice_cmplx& slice);

    arr_cmplx(const std::initializer_list<cmplx_t>& list) {
        _vec = std::vector<cmplx_t>(list);
    }

    template<typename T>
    explicit arr_cmplx(const T* x, size_t nx) {
        static_assert(std::is_convertible<T, cmplx_t>::value, "Type is not convertible to dsplib::cmplx_t");
        _vec.resize(nx);
        for (size_t i = 0; i < nx; ++i) {
            _vec[i] = x[i];
        }
    }

    //from [1, 2, 3, 4] to [{1, 2j}, {3, 4j}]
    template<typename T>
    static arr_cmplx pack(const T* x, size_t nx) {
        if (nx % 2 != 0) {
            throw(std::runtime_error("Array size is not even"));
        }

        const T* p = x;
        arr_cmplx r(nx / 2);
        for (size_t i = 0; i < r.size(); i++) {
            r[i].re = *(p++);
            r[i].im = *(p++);
        }
        return r;
    }

    template<typename T>
    static arr_cmplx pack(const std::vector<T>& arr) {
        static_assert(std::is_scalar<T>::value, "Type is not scalar");
        return arr_cmplx::pack(arr.data(), arr.size());
    }

    template<typename T>
    std::vector<T> unpack() const {
        static_assert(std::is_scalar<T>::value, "Type is not scalar");
        static_assert(std::is_convertible<real_t, T>::value, "Type is not convertible");
        std::vector<T> res(_vec.size() * 2);
        for (size_t i = 0; i < _vec.size(); i++) {
            res[2 * i] = _vec[i].re;
            res[2 * i + 1] = _vec[i].im;
        }
        return res;
    }

    explicit arr_cmplx(const std::vector<cmplx_t>& v);
    explicit arr_cmplx(int n);

    typedef std::vector<cmplx_t>::iterator iterator;
    typedef std::vector<cmplx_t>::const_iterator const_iterator;

    iterator begin() {
        return _vec.begin();
    }

    iterator end() {
        return _vec.end();
    }

    const_iterator begin() const {
        return _vec.begin();
    }

    const_iterator end() const {
        return _vec.end();
    }

    void push_back(const cmplx_t& v) {
        _vec.push_back(v);
    }

    void push_front(const cmplx_t& v) {
        _vec.insert(_vec.begin(), v);
    }

    cmplx_t pop_back() {
        auto r = _vec.back();
        _vec.pop_back();
        return r;
    }

    cmplx_t pop_front() {
        auto r = _vec.front();
        memmove(_vec.data(), _vec.data() + 1, (_vec.size() - 1) * sizeof(cmplx_t));
        _vec.resize(_vec.size() - 1);
        return r;
    }

    cmplx_t* data();
    const cmplx_t* data() const;

    int size() const {
        return _vec.size();
    }

    arr_cmplx& operator = (const arr_cmplx& rhs);
    arr_cmplx& operator = (arr_cmplx &&rhs);

    arr_cmplx& operator = (const slice_cmplx &rhs) {
        *this = arr_cmplx(rhs);
        return *this;
    }

    const cmplx_t& operator [] (int i) const;
    cmplx_t& operator [] (int i);

    arr_cmplx& operator + ();
    arr_cmplx& operator - ();

    arr_cmplx& operator += (const arr_cmplx& rhs);
    arr_cmplx& operator -= (const arr_cmplx& rhs);
    arr_cmplx& operator *= (const arr_cmplx& rhs);
    arr_cmplx& operator /= (const arr_cmplx& rhs);

    arr_cmplx operator + (const arr_cmplx& rhs) const;
    arr_cmplx operator - (const arr_cmplx& rhs) const;
    arr_cmplx operator * (const arr_cmplx& rhs) const;
    arr_cmplx operator / (const arr_cmplx& rhs) const;

    arr_cmplx& operator += (const cmplx_t& rhs);
    arr_cmplx& operator -= (const cmplx_t& rhs);
    arr_cmplx& operator *= (const cmplx_t& rhs);
    arr_cmplx& operator /= (const cmplx_t& rhs);

    arr_cmplx operator + (const cmplx_t& rhs) const;
    arr_cmplx operator - (const cmplx_t& rhs) const;
    arr_cmplx operator * (const cmplx_t& rhs) const;
    arr_cmplx operator / (const cmplx_t& rhs) const;

    arr_cmplx& operator += (const arr_real& rhs);
    arr_cmplx& operator -= (const arr_real& rhs);
    arr_cmplx& operator *= (const arr_real& rhs);
    arr_cmplx& operator /= (const arr_real& rhs);

    arr_cmplx operator + (const arr_real& rhs) const;
    arr_cmplx operator - (const arr_real& rhs) const;
    arr_cmplx operator * (const arr_real& rhs) const;
    arr_cmplx operator / (const arr_real& rhs) const;

    arr_cmplx& operator += (const real_t& rhs);
    arr_cmplx& operator -= (const real_t& rhs);
    arr_cmplx& operator *= (const real_t& rhs);
    arr_cmplx& operator /= (const real_t& rhs);

    arr_cmplx operator + (const real_t& rhs) const;
    arr_cmplx operator - (const real_t& rhs) const;
    arr_cmplx operator * (const real_t& rhs) const;
    arr_cmplx operator / (const real_t& rhs) const;

    bool empty() const;
    
    slice_cmplx slice(int i1, int i2, int m=1);

private:
    std::vector <cmplx_t> _vec;
};

//--------------------------------------------------------------------------------
inline arr_cmplx operator + (const real_t& lhs, const arr_cmplx& rhs) {
    return rhs + lhs;
}

inline arr_cmplx operator - (const real_t& lhs, const arr_cmplx& rhs) {
    arr_cmplx r(rhs);
    return -r + lhs;
}

inline arr_cmplx operator * (const real_t& lhs, const arr_cmplx& rhs) {
    return rhs * lhs;
}

inline arr_cmplx operator / (const real_t& lhs, const arr_cmplx& rhs) {
    arr_cmplx r(rhs);
    int n = r.size();
    for (int i=0; i < n; ++i) {
        r[i] = lhs / rhs[i];
    }

    return r;
}

//--------------------------------------------------------------------------------
inline arr_real operator + (const real_t& lhs, const arr_real& rhs) {
    return rhs + lhs;
}

inline arr_cmplx operator + (const cmplx_t& lhs, const arr_real& rhs) {
    return rhs + lhs;
}

inline arr_real operator - (const real_t& lhs, const arr_real& rhs) {
    arr_real r(rhs);
    return -r + lhs;
}

inline arr_cmplx operator - (const cmplx_t& lhs, const arr_real& rhs) {
    arr_cmplx r(rhs);
    return -r + lhs;
}

inline arr_real operator * (const real_t& lhs, const arr_real& rhs) {
    return rhs * lhs;
}

inline arr_cmplx operator * (const cmplx_t& lhs, const arr_real& rhs) {
    return rhs * lhs;
}

inline arr_real operator / (const real_t& lhs, const arr_real& rhs) {
    arr_real r(rhs.size());
    for (int i=0; i < r.size(); ++i) {
        r[i] = lhs / rhs[i];
    }

    return r;
}

inline arr_cmplx operator / (const cmplx_t& lhs, const arr_real& rhs) {
    arr_cmplx r(rhs.size());
    for (int i=0; i < r.size(); ++i) {
        r[i] = lhs / rhs[i];
    }

    return r;
}

inline arr_cmplx operator * (const arr_real& lhs, const std::complex<double>& rhs) {
    return lhs * cmplx_t(rhs);
}

inline arr_cmplx operator * (const std::complex<double>& lhs, const arr_real& rhs) {
    return rhs * cmplx_t(lhs);
}

inline arr_cmplx operator * (const arr_real& lhs, _Complex double rhs) {
    return lhs * cmplx_t(rhs);
}

inline arr_cmplx operator * (_Complex double lhs, const arr_real& rhs) {
    return rhs * cmplx_t(lhs);
}

} ///< dsplib
