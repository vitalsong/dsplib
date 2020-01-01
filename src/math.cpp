#include <dsplib/math.h>

#include <stdexcept>
#include <algorithm>
#include <math.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
real_t max(const arr_real &arr) {
    return *std::max_element(arr.begin(), arr.end());
}

//-------------------------------------------------------------------------------------------------
cmplx_t max(const arr_cmplx &arr) {
    return *std::max_element(arr.begin(), arr.end());
}

//-------------------------------------------------------------------------------------------------
int argmax(const arr_real& arr) {
    return std::distance(arr.begin(), std::max_element(arr.begin(), arr.end()));
}

//-------------------------------------------------------------------------------------------------
int argmax(const arr_cmplx& arr) {
    return std::distance(arr.begin(), std::max_element(arr.begin(), arr.end()));
}

//-------------------------------------------------------------------------------------------------
real_t min(const arr_real &arr) {
    return *std::min_element(arr.begin(), arr.end());
}

//-------------------------------------------------------------------------------------------------
cmplx_t min(const arr_cmplx &arr) {
    return *std::min_element(arr.begin(), arr.end());
}

//-------------------------------------------------------------------------------------------------
int argmin(const arr_real& arr) {
    return std::distance(arr.begin(), std::min_element(arr.begin(), arr.end()));
}

//-------------------------------------------------------------------------------------------------
int argmin(const arr_cmplx& arr) {
    return std::distance(arr.begin(), std::min_element(arr.begin(), arr.end()));
}

//-------------------------------------------------------------------------------------------------
arr_real abs(const arr_real &arr)
{
    arr_real r = arr;
    const int N = arr.size();
    for (int i=0; i < N; ++i) {
        r[i] = ::fabsl(arr[i]);
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
real_t abs(real_t v)
{
    return ::fabsl(v);
}

//-------------------------------------------------------------------------------------------------
arr_real abs(const arr_cmplx &arr)
{
    arr_real r(arr.size());
    const int N = arr.size();
    for (int i=0; i < N; ++i) {
        r[i] = sqrt(arr[i].xi * arr[i].xi + arr[i].xq * arr[i].xq);
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
real_t abs(cmplx_t v)
{
    return sqrt(v.xi * v.xi + v.xq * v.xq);
}

//-------------------------------------------------------------------------------------------------
arr_real fabs(const arr_cmplx &arr)
{
    arr_real r(arr.size());
    const int N = arr.size();
    for (int i=0; i < N; ++i) {
        r[i] = fabs(arr[i]);
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
real_t fabs(cmplx_t v)
{
    double min, max;
    if (abs(v.xi) > abs(v.xq))
    {
        min = abs(v.xq);
        max = abs(v.xi);
    }
    else
    {
        min = abs(v.xi);
        max = abs(v.xq);
    }

    return max + (min / 2);
}

//-------------------------------------------------------------------------------------------------
arr_real round(const arr_real &arr)
{
    arr_real r(arr.size());
    const int N = arr.size();
    for (int i=0; i < N; ++i) {
        r[i] = ::round(arr[i]);
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx round(const arr_cmplx &arr)
{
    arr_cmplx r(arr.size());
    const int N = arr.size();
    for (int i=0; i < N; ++i)
    {
        r[i].xi = ::round(arr[i].xi);
        r[i].xq = ::round(arr[i].xq);
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
real_t sum(const arr_real &arr)
{
    real_t acc = 0;
    const int N = arr.size();
    for (int i=0; i < N; ++i) {
        acc += arr[i];
    }

    return acc;
}

//-------------------------------------------------------------------------------------------------
cmplx_t sum(const arr_cmplx &arr)
{
    cmplx_t acc = 0;
    const int N = arr.size();
    for (int i=0; i < N; ++i)
    {
        acc.xi += arr[i].xi;
        acc.xq += arr[i].xq;
    }

    return acc;
}

//-------------------------------------------------------------------------------------------------
real_t mean(const arr_real &arr)
{
    real_t s = sum(arr);
    return s / arr.size();
}

//-------------------------------------------------------------------------------------------------
cmplx_t mean(const arr_cmplx &arr)
{
    cmplx_t s = sum(arr);
    return s / arr.size();
}

//-------------------------------------------------------------------------------------------------
real_t stddev(const arr_real &arr)
{
    real_t m = mean(arr);
    real_t r = sqrt(mean(pow2(arr - m)));
    return r;
}

//-------------------------------------------------------------------------------------------------
real_t median(const arr_real &arr)
{
    arr_real r(arr);
    std::sort(r.begin(), r.end());
    return r[r.size()/2];
}

//-------------------------------------------------------------------------------------------------
arr_real real(const arr_cmplx &x)
{
    arr_real r(x.size());
    for (int i=0; i < x.size(); ++i) {
        r[i] = x[i].xi;
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
real_t real(cmplx_t x)
{
    return x.xi;
}

//-------------------------------------------------------------------------------------------------
arr_real imag(const arr_cmplx &x)
{
    arr_real r(x.size());
    for (int i=0; i < x.size(); ++i) {
        r[i] = x[i].xq;
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
real_t imag(cmplx_t x)
{
    return x.xq;
}

//-------------------------------------------------------------------------------------------------
void zeroing(arr_cmplx &arr)
{
    const int N = arr.size();
    for (int i=0; i < N; ++i)
    {
        arr[i].xi = 0;
        arr[i].xq = 0;
    }
}

//-------------------------------------------------------------------------------------------------
void zeroing(arr_real &arr)
{
    const int N = arr.size();
    for (int i=0; i < N; ++i)
    {
        arr[i] = 0;
    }
}

//-------------------------------------------------------------------------------------------------
int nextpow2(int m)
{
    if ((m == 0) || (m == 1)) {
        return 0;
    }

    int p = 0;
    while ((m >> p) != 0) {
        ++p;
    }

    --p;
    if ((int(1) << p) == m) {
        return p;
    }
    else {
        return p+1;
    }
}

//-------------------------------------------------------------------------------------------------
arr_cmplx complex(const arr_real &re, const arr_real &im)
{
    if (re.size() != im.size()) {
        throw std::invalid_argument("array sizes are different");
    }

    int n = re.size();
    arr_cmplx r(n);
    for (int i=0; i < n; ++i)
    {
        r[i].xi = re[i];
        r[i].xq = im[i];
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real log(const arr_real &arr)
{
    int n = arr.size();
    arr_real r(n);
    for (int i=0; i < n; ++i) {
        r[i] = ::log(arr[i]);
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real log2(const arr_real &arr)
{
    int n = arr.size();
    arr_real r(n);
    for (int i=0; i < n; ++i) {
        r[i] = ::log2(arr[i]);
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real log10(const arr_real &arr)
{
    int n = arr.size();
    arr_real r(n);
    for (int i=0; i < n; ++i) {
        r[i] = ::log10(arr[i]);
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
real_t rms(const arr_real &arr)
{
    const int N = arr.size();
    real_t sum = 0;
    for (int i=0; i < N; ++i) {
        sum += (arr[i] * arr[i]);
    }

    return sqrt(sum/N);
}

//-------------------------------------------------------------------------------------------------
real_t rms(const arr_cmplx &arr)
{
    const int N = arr.size();
    real_t sum = 0;
    for (int i=0; i < N; ++i)
    {
        sum += (arr[i].xi * arr[i].xi);
        sum += (arr[i].xq * arr[i].xq);
    }

    return sqrt(sum/N);
}

//-------------------------------------------------------------------------------------------------
arr_real sin(const arr_real &arr)
{
    int n = arr.size();
    arr_real r(n);
    for (int i=0; i < n; ++i) {
        r[i] = ::sin(arr[i]);
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real cos(const arr_real &arr)
{
    int n = arr.size();
    arr_real r(n);
    for (int i=0; i < n; ++i) {
        r[i] = ::cos(arr[i]);
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx conj(const arr_cmplx &x)
{
    int n = x.size();
    arr_cmplx r = x;
    for (int i=0; i < n; ++i) {
        r[i].xq = -r[i].xq;
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
cmplx_t conj(cmplx_t x)
{
    x.xq = -x.xq;
    return x;
}

//-------------------------------------------------------------------------------------------------
real_t apprx(real_t y0, real_t y1, real_t y2)
{
    double a = ((y2 - y1) + (y0 - y1)) / 2;
    double b = y1 - a - y0;
    double q = (-b);
    return q / (2 * a) - 1;
}

//-------------------------------------------------------------------------------------------------
arr_real pow2(const arr_real &arr)
{
    arr_real r(arr);
    for (int i=0; i < arr.size(); ++i) {
        r[i] *= r[i];
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx pow2(const arr_cmplx &arr)
{
    arr_cmplx r(arr);
    for (int i=0; i < arr.size(); ++i) {
        r[i] *= r[i];
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real angle(const arr_cmplx &arr)
{
    arr_real r(arr.size());
    for (int i=0; i < r.size(); ++i) {
        r[i] = angle(arr[i]);
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
real_t angle(cmplx_t v)
{
    real_t d = 0;
    if (v.xi < 0) {
        d = (v.xq > 0) ? (M_PI) : (-M_PI);
    }
    else {
        d = 0;
    }

    return ::atan(v.xq / v.xi) + d;
}

} ///< dsplib


