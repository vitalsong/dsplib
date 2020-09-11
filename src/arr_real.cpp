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
int arr_real::size() const {
    return _vec.size();
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
void arr_real::set(int p1, int p2, const arr_real &arr)
{
    int n = (p2 - p1 + 1);

    if (((p1 + n) > this->size()) || (n > arr.size())) {
        throw std::overflow_error("array size error");
    }

    for (int i=0; i < n; ++i) {
        _vec[p1+i] = arr[i];
    }
}

//-------------------------------------------------------------------------------------------------
void arr_real::set(int p1, int p2, real_t value)
{
    int n = (p2 - p1 + 1);

    if ((p1 + n) > this->size()) {
        throw std::overflow_error("array size error");
    }

    for (int i=0; i < n; ++i) {
        _vec[p1+i] = value;
    }
}

//-------------------------------------------------------------------------------------------------
arr_real arr_real::slice(int i1, int i2) const
{
    int n = i2 - i1;
    arr_real r(n);
    for (int i=0; i < n; ++i) {
        r[i] = _vec[i+i1];
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real arr_real::zeros(int n)
{
    arr_real r(n);
    std::fill(r.data(), r.data() + n, 0);
    return r;
}

}   ///< dsplab
