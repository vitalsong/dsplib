#include <dsplib/array.h>
#include <stdexcept>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
arr_cmplx::arr_cmplx() {}

//-------------------------------------------------------------------------------------------------
arr_cmplx::arr_cmplx(const arr_cmplx &v) : _vec(v._vec) {}

//-------------------------------------------------------------------------------------------------
arr_cmplx::arr_cmplx(arr_cmplx &&v) : _vec(std::move(v._vec)) {}

//-------------------------------------------------------------------------------------------------
arr_cmplx::arr_cmplx(const arr_real& v)
{
    _vec.resize(v.size());
    for (size_t i = 0; i < v.size(); i++)
    {
        _vec[i].re = v[i];
        _vec[i].im = 0;
    }
}

//-------------------------------------------------------------------------------------------------
arr_cmplx::arr_cmplx(const std::vector<cmplx_t> &v) : arr_cmplx(v.data(), v.size()) {}

//-------------------------------------------------------------------------------------------------
arr_cmplx::arr_cmplx(int n) {
    _vec.resize(n);
    std::fill(_vec.data(), _vec.data() + n, 0);
}

//-------------------------------------------------------------------------------------------------
cmplx_t *arr_cmplx::data() {
    return _vec.data();
}

//-------------------------------------------------------------------------------------------------
const cmplx_t *arr_cmplx::data() const {
    return _vec.data();
}

//-------------------------------------------------------------------------------------------------
int arr_cmplx::size() const {
    return _vec.size();
}

//-------------------------------------------------------------------------------------------------
arr_cmplx &arr_cmplx::operator =(const arr_cmplx &rhs)
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
arr_cmplx &arr_cmplx::operator =(arr_cmplx &&rhs)
{
    if (this == &rhs) {
        return *this;
    }

    _vec.swap(rhs._vec);
    return *this;
}

//-------------------------------------------------------------------------------------------------
const cmplx_t &arr_cmplx::operator [](int i) const {
    return *(_vec.data() + i);
}

//-------------------------------------------------------------------------------------------------
cmplx_t &arr_cmplx::operator [](int i) {
    return *(_vec.data() + i);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx &arr_cmplx::operator +() {
    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx &arr_cmplx::operator -()
{
    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i] = (-_vec[i]);
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx &arr_cmplx::operator +=(const arr_cmplx &rhs)
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
arr_cmplx &arr_cmplx::operator -=(const arr_cmplx &rhs)
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
arr_cmplx &arr_cmplx::operator *=(const arr_cmplx &rhs)
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
arr_cmplx &arr_cmplx::operator /=(const arr_cmplx &rhs)
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
arr_cmplx arr_cmplx::operator +(const arr_cmplx &rhs) const
{
    arr_cmplx temp = *this;
    temp += rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_cmplx::operator -(const arr_cmplx &rhs) const
{
    arr_cmplx temp = *this;
    temp -= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_cmplx::operator *(const arr_cmplx &rhs) const
{
    arr_cmplx temp = *this;
    temp *= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_cmplx::operator /(const arr_cmplx &rhs) const
{
    arr_cmplx temp = *this;
    temp /= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx &arr_cmplx::operator +=(const cmplx_t &rhs)
{
    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i] += rhs;
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx &arr_cmplx::operator -=(const cmplx_t &rhs)
{
    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i] -= rhs;
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx &arr_cmplx::operator *=(const cmplx_t &rhs)
{
    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i] *= rhs;
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx &arr_cmplx::operator /=(const cmplx_t &rhs)
{
    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i] /= rhs;
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_cmplx::operator +(const cmplx_t &rhs) const
{
    arr_cmplx temp = *this;
    temp += rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_cmplx::operator -(const cmplx_t &rhs) const
{
    arr_cmplx temp = *this;
    temp -= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_cmplx::operator *(const cmplx_t &rhs) const
{
    arr_cmplx temp = *this;
    temp *= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_cmplx::operator /(const cmplx_t &rhs) const
{
    arr_cmplx temp = *this;
    temp /= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx &arr_cmplx::operator +=(const arr_real &rhs)
{
    if (this->size() != rhs.size()) {
        throw std::invalid_argument("array sizes are different");
    }

    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i].re += rhs[i];
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx &arr_cmplx::operator -=(const arr_real &rhs)
{
    if (this->size() != rhs.size()) {
        throw std::invalid_argument("array sizes are different");
    }

    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i].re -= rhs[i];
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx &arr_cmplx::operator *=(const arr_real &rhs)
{
    if (this->size() != rhs.size()) {
        throw std::invalid_argument("array sizes are different");
    }

    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i] *= rhs[i];
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx &arr_cmplx::operator /=(const arr_real &rhs)
{
    if (this->size() != rhs.size()) {
        throw std::invalid_argument("array sizes are different");
    }

    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i] /= rhs[i];
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_cmplx::operator +(const arr_real &rhs) const
{
    arr_cmplx temp = *this;
    temp += rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_cmplx::operator -(const arr_real &rhs) const
{
    arr_cmplx temp = *this;
    temp -= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_cmplx::operator *(const arr_real &rhs) const
{
    arr_cmplx temp = *this;
    temp *= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_cmplx::operator /(const arr_real &rhs) const
{
    arr_cmplx temp = *this;
    temp /= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx &arr_cmplx::operator +=(const real_t &rhs)
{
    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i].re += rhs;
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx &arr_cmplx::operator -=(const real_t &rhs)
{
    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i].re -= rhs;
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx &arr_cmplx::operator *=(const real_t &rhs)
{
    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i] *= rhs;
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx &arr_cmplx::operator /=(const real_t &rhs)
{
    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i] /= rhs;
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_cmplx::operator +(const real_t &rhs) const
{
    arr_cmplx temp = *this;
    temp += rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_cmplx::operator -(const real_t &rhs) const
{
    arr_cmplx temp = *this;
    temp -= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_cmplx::operator *(const real_t &rhs) const
{
    arr_cmplx temp = *this;
    temp *= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_cmplx::operator /(const real_t &rhs) const
{
    arr_cmplx temp = *this;
    temp /= rhs;
    return temp;
}

//-------------------------------------------------------------------------------------------------
bool arr_cmplx::empty() const
{
    return _vec.empty();
}

//-------------------------------------------------------------------------------------------------
void arr_cmplx::set(int p1, int p2, const arr_cmplx &arr)
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
void arr_cmplx::set(int p1, int p2, cmplx_t value)
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
arr_cmplx arr_cmplx::slice(int i1, int i2) const
{
    int n = i2 - i1;
    arr_cmplx r(n);
    for (int i=0; i < n; ++i) {
        r[i] = _vec[i+i1];
    }

    return r;
}

} ///< dsplib
