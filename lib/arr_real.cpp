#include <dsplib/array.h>
#include <stdexcept>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
arr_real::arr_real() {}

//-------------------------------------------------------------------------------------------------
arr_real::arr_real(const arr_real &v) : _vec(v._vec) {}

//-------------------------------------------------------------------------------------------------
arr_real::arr_real(arr_real &&v) : _vec(std::move(v._vec)){}

//-------------------------------------------------------------------------------------------------
arr_real::arr_real(const std::vector<real_t> &v) : arr_real(v.data(), v.size()) {}

//-------------------------------------------------------------------------------------------------
arr_real::arr_real(int n) {
    _vec.resize(n);
    std::fill(_vec.data(), _vec.data() + n, 0);
}

//-------------------------------------------------------------------------------------------------
arr_real::arr_real(const slice_real& slice) {
    _vec.resize((slice.p2 - slice.p1) / slice.step);
    auto r = this->slice(0, _vec.size());
    r = slice;
}

//-------------------------------------------------------------------------------------------------
real_t *arr_real::data() {
    return _vec.data();
}

//-------------------------------------------------------------------------------------------------
const real_t *arr_real::data() const {
    return _vec.data();
}

//-------------------------------------------------------------------------------------------------
arr_real &arr_real::operator = (const arr_real &rhs)
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

//-------------------------------------------------------------------------------------------------
arr_real &arr_real::operator = (arr_real &&rhs)
{
    if (this == &rhs) {
        return *this;
    }

    _vec.swap(rhs._vec);
    return *this;
}

//-------------------------------------------------------------------------------------------------
const real_t &arr_real::operator [](int i) const {
    if (i >= 0) {
        return _vec[i];
    } else {
        return _vec[_vec.size() + i];
    }
}

//-------------------------------------------------------------------------------------------------
real_t &arr_real::operator [](int i) {
    if (i >= 0) {
        return _vec[i];
    } else {
        return _vec[_vec.size() + i];
    }
}

//-------------------------------------------------------------------------------------------------
arr_real &arr_real::operator +() {
    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_real &arr_real::operator -()
{
    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i] = (-_vec[i]);
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_real &arr_real::operator +=(const arr_real &rhs)
{
    if (this->size() != rhs.size()) {
        throw std::invalid_argument("array sizes are different");
    }

    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i] += rhs._vec[i];
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_real &arr_real::operator -=(const arr_real &rhs)
{
    if (this->size() != rhs.size()) {
        throw std::invalid_argument("array sizes are different");
    }

    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i] -= rhs._vec[i];
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_real &arr_real::operator *=(const arr_real &rhs)
{
    if (this->size() != rhs.size()) {
        throw std::invalid_argument("array sizes are different");
    }

    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i] *= rhs._vec[i];
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_real &arr_real::operator /=(const arr_real &rhs)
{
    if (this->size() != rhs.size()) {
        throw std::invalid_argument("array sizes are different");
    }

    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i] /= rhs._vec[i];
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_real arr_real::operator +(const arr_real &rhs) const
{
    arr_real temp = *this;
    temp += rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_real arr_real::operator -(const arr_real &rhs) const
{
    arr_real temp = *this;
    temp -= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_real arr_real::operator *(const arr_real &rhs) const
{
    arr_real temp = *this;
    temp *= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_real arr_real::operator /(const arr_real &rhs) const
{
    arr_real temp = *this;
    temp /= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_real::operator +(const arr_cmplx &rhs) const
{
    arr_cmplx temp(rhs);
    temp += *this;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_real::operator -(const arr_cmplx &rhs) const
{
    arr_cmplx temp(rhs);
    temp -= *this;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_real::operator *(const arr_cmplx &rhs) const
{
    arr_cmplx temp(rhs);
    temp *= *this;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_real::operator /(const arr_cmplx &rhs) const
{
    arr_cmplx temp(rhs);
    temp /= *this;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_real &arr_real::operator +=(const real_t &rhs)
{
    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i] += rhs;
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_real &arr_real::operator -=(const real_t &rhs)
{
    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i] -= rhs;
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_real &arr_real::operator *=(const real_t &rhs)
{
    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i] *= rhs;
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_real &arr_real::operator /=(const real_t &rhs)
{
    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i] /= rhs;
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_real arr_real::operator +(const real_t &rhs) const
{
    arr_real temp = *this;
    temp += rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_real arr_real::operator -(const real_t &rhs) const
{
    arr_real temp = *this;
    temp -= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_real arr_real::operator *(const real_t &rhs) const
{
    arr_real temp = *this;
    temp *= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_real arr_real::operator /(const real_t &rhs) const
{
    arr_real temp = *this;
    temp /= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_real::operator +(const cmplx_t &rhs) const
{
    arr_cmplx temp(_vec.data(), _vec.size());
    temp += rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_real::operator -(const cmplx_t &rhs) const
{
    arr_cmplx temp(_vec.data(), _vec.size());
    temp -= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_real::operator *(const cmplx_t &rhs) const
{
    arr_cmplx temp(_vec.data(), _vec.size());
    temp *= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_real::operator /(const cmplx_t &rhs) const
{
    arr_cmplx temp(_vec.data(), _vec.size());
    temp /= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
bool arr_real::empty() const
{
    return _vec.empty();
}

//-------------------------------------------------------------------------------------------------
slice_real arr_real::slice(int i1, int i2, int m)
{
    if (m < 1) {
        throw std::range_error("Step must be greater 1");
    }

    if (i2 - i1 > _vec.size()) {
        throw std::range_error("Slice range is greater vector size");
    }

    slice_real r;
    r.p1 = i1;
    r.p2 = i2;
    r.step = m;
    r.data = _vec.data();
    r.size = _vec.size();
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real &arr_real::operator ^=(const real_t &base)
{
    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i] = pow(_vec[i], base);
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_real arr_real::operator ^(const real_t &base) const
{
    arr_real temp(_vec.data(), _vec.size());
    temp ^= base;
    return temp;
}

}   ///< dsplab
