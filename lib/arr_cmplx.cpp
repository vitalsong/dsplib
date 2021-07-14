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
arr_cmplx::arr_cmplx(const slice_cmplx& slice) {
    _vec.resize((slice.p2 - slice.p1) / slice.step);
    auto r = this->slice(0, _vec.size());
    r = slice;
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
    if (i >= 0) {
        return _vec[i];
    } else {
        return _vec[_vec.size() + i];
    }
}

//-------------------------------------------------------------------------------------------------
cmplx_t &arr_cmplx::operator [](int i) {
    if (i >= 0) {
        return _vec[i];
    } else {
        return _vec[_vec.size() + i];
    }
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
slice_cmplx arr_cmplx::slice(int i1, int i2, int m)
{
    if (m < 1) {
        throw std::range_error("Step must be greater 1");
    }

    if (i2 - i1 > _vec.size()) {
        throw std::range_error("Slice range is greater vector size");
    }

    slice_cmplx r;
    r.p1 = i1;
    r.p2 = i2;
    r.step = m;
    r.data = _vec.data();
    r.size = _vec.size();
    return r;
}

} ///< dsplib
