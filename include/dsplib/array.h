#ifndef ARRAY_H
#define ARRAY_H

#include <dsplib/types.h>

namespace dsplib {

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
    explicit arr_real(const T* x, int nx)
    {
        _vec.resize(nx);
        for (int i=0; i < nx; ++i) {
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

    arr_real& operator += (const real_t& rhs);
    arr_real& operator -= (const real_t& rhs);
    arr_real& operator *= (const real_t& rhs);
    arr_real& operator /= (const real_t& rhs);

    arr_real operator + (const real_t& rhs) const;
    arr_real operator - (const real_t& rhs) const;
    arr_real operator * (const real_t& rhs) const;
    arr_real operator / (const real_t& rhs) const;

    bool empty() const;

    void set(int p1, int p2, const arr_real& arr);
    void set(int p1, int p2, real_t value);
    arr_real slice(int i1, int i2) const;

    static arr_real join(const arr_real& lhs, const arr_real& rhs);
    static arr_real zeros(int n);
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

    template <typename T>
    explicit arr_cmplx(const T* x, int nx)
    {
        _vec.resize(nx);
        for (int i=0; i < nx; ++i) {
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

    arr_cmplx& operator *= (const arr_real& rhs);
    arr_cmplx& operator /= (const arr_real& rhs);
    arr_cmplx operator * (const arr_real& rhs) const;
    arr_cmplx operator / (const arr_real& rhs) const;

    arr_cmplx& operator *= (const real_t& rhs);
    arr_cmplx& operator /= (const real_t& rhs);
    arr_cmplx operator * (const real_t& rhs) const;
    arr_cmplx operator / (const real_t& rhs) const;

    bool empty() const;
    void set(int p1, int p2, const arr_cmplx& arr);
    void set(int p1, int p2, cmplx_t value);
    arr_cmplx slice(int i1, int i2) const;

    static arr_cmplx join(const arr_cmplx& lhs, const arr_cmplx& rhs);
    static arr_cmplx zeros(int n);
    static arr_cmplx init(const std::initializer_list<dsplib::cmplx_t>& list) {
        return arr_cmplx(std::vector<dsplib::cmplx_t>(list));
    }

private:
    std::vector <cmplx_t> _vec;
};

} ///< dsplib

#endif // ARRAY_H
