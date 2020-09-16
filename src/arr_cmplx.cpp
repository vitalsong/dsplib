#include <dsplib/array.h>
#include <stdexcept>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
void slice_cmplx::operator = (const slice_cmplx &rhs) 
{
    int n1 = (p2 - p1 + 1) / step;
    int n2 = (rhs.p2 - rhs.p1 + 1) / rhs.step;
    if (n1 != n2) {
        throw std::range_error("Not equal size");
    }
    cmplx_t* d1 = data + p1;
    const cmplx_t* d2 = rhs.data + rhs.p1;
    for (size_t i = 0; i < n2; i++) {
        *d1 = *d2;
        d1 += step;
        d2 += rhs.step;
    }
}

//-------------------------------------------------------------------------------------------------
void slice_cmplx::operator = (const arr_cmplx &rhs)
{
    const int n1 = (p2 - p1 + 1) / step;
    const int n2 = rhs.size();
    if (n1 != n2) {
        throw std::range_error("Not equal size");
    }
    cmplx_t* d1 = data + p1;
    const cmplx_t* d2 = rhs.data();
    for (size_t i = 0; i < n2; i++) {
        *d1 = *d2;
        d1 += step;
        d2 += 1;
    }
}

//-------------------------------------------------------------------------------------------------
void slice_cmplx::operator = (const cmplx_t &value) 
{
    const int n = (p2 - p1 + 1) / step;
    cmplx_t* d1 = data + p1;
    for (size_t i = 0; i < n; i++) {
        *d1 = value;
        d1 += step;
    }
}

//-------------------------------------------------------------------------------------------------
void slice_cmplx::operator = (const std::initializer_list<cmplx_t> &list)
{
    const int n1 = (p2 - p1 + 1) / step;
    const int n2 = list.size();
    if (n1 != n2) {
        throw std::range_error("Not equal size");
    }
    cmplx_t* d1 = data + p1;
    const cmplx_t* d2 = list.begin();
    for (size_t i = 0; i < n2; i++) {
        *d1 = *d2;
        d1 += step;
        d2 += 1;
    }
}

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
    const int n1 = (slice.p2 - slice.p1 + 1) / slice.step;
    const int n2 = _vec.size();
    if (n1 != n2) {
        throw std::range_error("Not equal size");
    }
    const cmplx_t* d1 = slice.data + slice.p1;
    cmplx_t* d2 = _vec.data();
    for (size_t i = 0; i < n2; i++) {
        *d2 = *d1;
        d1 += slice.step;
        d2 += 1;
    }
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
slice_cmplx arr_cmplx::slice(int i1, int i2)
{
    slice_cmplx r;
    r.p1 = i1;
    r.p2 = i2;
    r.step = 1;
    r.data = _vec.data();
    return r;
}

} ///< dsplib
