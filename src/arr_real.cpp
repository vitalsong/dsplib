#include <dsplib/array.h>
#include <stdexcept>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
void slice_real::operator = (const slice_real &rhs) 
{
    int n1 = (p2 - p1) / step;
    int n2 = (rhs.p2 - rhs.p1) / rhs.step;
    if (n1 != n2) {
        throw std::range_error("Not equal size");
    }
    real_t* d1 = data + p1;
    const real_t* d2 = rhs.data + rhs.p1;
    for (size_t i = 0; i < n2; i++) {
        *d1 = *d2;
        d1 += step;
        d2 += rhs.step;
    }
}

//-------------------------------------------------------------------------------------------------
void slice_real::operator = (const arr_real &rhs)
{
    const int n1 = (p2 - p1) / step;
    const int n2 = rhs.size();
    if (n1 != n2) {
        throw std::range_error("Not equal size");
    }
    real_t* d1 = data + p1;
    const real_t* d2 = rhs.data();
    for (size_t i = 0; i < n2; i++) {
        *d1 = *d2;
        d1 += step;
        d2 += 1;
    }
}

//-------------------------------------------------------------------------------------------------
void slice_real::operator = (const real_t &value) 
{
    const int n = (p2 - p1) / step;
    real_t* d1 = data + p1;
    for (size_t i = 0; i < n; i++) {
        *d1 = value;
        d1 += step;
    }
}

//-------------------------------------------------------------------------------------------------
void slice_real::operator = (const std::initializer_list<real_t> &list)
{
    const int n1 = (p2 - p1) / step;
    const int n2 = list.size();
    if (n1 != n2) {
        throw std::range_error("Not equal size");
    }
    real_t* d1 = data + p1;
    const real_t* d2 = list.begin();
    for (size_t i = 0; i < n2; i++) {
        *d1 = *d2;
        d1 += step;
        d2 += 1;
    }
}

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
    const int n = (slice.p2 - slice.p1) / slice.step;
    _vec.resize(n);
    const real_t* d1 = slice.data + slice.p1;
    real_t* d2 = _vec.data();
    for (size_t i = 0; i < n; i++) {
        *d2 = *d1;
        d1 += slice.step;
        d2 += 1;
    }
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
    return *(_vec.data() + i);
}

//-------------------------------------------------------------------------------------------------
real_t &arr_real::operator [](int i) {
    return *(_vec.data() + i);
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
slice_real arr_real::slice(int i1, int i2)
{
    slice_real r;
    r.p1 = i1;
    r.p2 = i2;
    r.step = 1;
    r.data = _vec.data();
    return r;
}

}   ///< dsplab
