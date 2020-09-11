#pragma once

#include <dsplib/types.h>
#include <vector>

namespace dsplib {

class arr_real;
class arr_cmplx;

/*!
 * \brief Real array
 */
class arr_real
{
public:

    arr_real();
    arr_real(const arr_real& v);
    arr_real(arr_real&& v);

    template <typename T>
    explicit arr_real(const T* x, size_t nx)
    {
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

    real_t* data();
    const real_t* data() const;
    int size() const;

    arr_real& operator = (const arr_real &rhs);
    arr_real& operator = (arr_real &&rhs);

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

    bool empty() const;

    void set(int p1, int p2, const arr_real& arr);
    void set(int p1, int p2, real_t value);
    arr_real slice(int i1, int i2) const;

    static arr_real init(const std::initializer_list<dsplib::real_t>& list) {
        return arr_real(std::vector<dsplib::real_t>(list));
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

    template <typename T>
    explicit arr_cmplx(const T* x, size_t nx)
    {
        _vec.resize(nx);
        for (size_t i=0; i < nx; ++i) {
            _vec[i] = x[i];
        }
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

    cmplx_t* data();
    const cmplx_t* data() const;
    int size() const;

    arr_cmplx& operator = (const arr_cmplx& rhs);
    arr_cmplx& operator = (arr_cmplx &&rhs);

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
    void set(int p1, int p2, const arr_cmplx& arr);
    void set(int p1, int p2, cmplx_t value);
    arr_cmplx slice(int i1, int i2) const;

    static arr_cmplx init(const std::initializer_list<dsplib::cmplx_t>& list) {
        return arr_cmplx(std::vector<dsplib::cmplx_t>(list));
    }

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
