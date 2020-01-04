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
arr_cmplx::arr_cmplx(const std::vector<cmplx_t> &v) : arr_cmplx(v.data(), v.size()) {}

//-------------------------------------------------------------------------------------------------
arr_cmplx::arr_cmplx(int n) {
    _vec.resize(n);
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
        _vec[i].xi += rhs[i];
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
        _vec[i].xi -= rhs[i];
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
        _vec[i].xi += rhs;
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx &arr_cmplx::operator -=(const real_t &rhs)
{
    int n = _vec.size();
    for (int i=0; i < n; ++i) {
        _vec[i].xi -= rhs;
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
    int n = i2 - i1 + 1;
    arr_cmplx r(n);
    for (int i=0; i < n; ++i) {
        r[i] = _vec[i+i1];
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_cmplx::join(const arr_cmplx &lhs, const arr_cmplx &rhs)
{
    int n1 = lhs.size();
    int n2 = rhs.size();
    arr_cmplx r(n1 + n2);

    for (int i=0; i < n1; ++i) {
        r[i] = lhs[i];
    }

    for (int i=0; i < n2; ++i) {
        r[i+n1] = rhs[i];
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx arr_cmplx::zeros(int n)
{
    arr_cmplx r(n);
    std::fill(r.data(), r.data() + n, 0);
    return r;
}

} ///< dsplib
