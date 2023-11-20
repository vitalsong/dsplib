#include "dsplib/array.h"
#include "dsplib/keywords.h"
#include "dsplib/math.h"
#include "dsplib/throw.h"
#include "dsplib/types.h"
#include "dsplib/utils.h"

#include <algorithm>
#include <cmath>
#include <complex>
#include <utility>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
real_t max(const arr_real& arr) {
    return *std::max_element(arr.begin(), arr.end());
}

cmplx_t max(const arr_cmplx& arr) {
    return *std::max_element(arr.begin(), arr.end());
}

//-------------------------------------------------------------------------------------------------
int argmax(const arr_real& arr) {
    return std::distance(arr.begin(), std::max_element(arr.begin(), arr.end()));
}

int argmax(const arr_cmplx& arr) {
    return std::distance(arr.begin(), std::max_element(arr.begin(), arr.end()));
}

//-------------------------------------------------------------------------------------------------
real_t min(const arr_real& arr) {
    return *std::min_element(arr.begin(), arr.end());
}

cmplx_t min(const arr_cmplx& arr) {
    return *std::min_element(arr.begin(), arr.end());
}

//-------------------------------------------------------------------------------------------------
real_t peak2peak(const arr_real& arr) {
    auto p = std::minmax_element(arr.begin(), arr.end());
    return (*p.second - *p.first);
}

cmplx_t peak2peak(const arr_cmplx& arr) {
    auto p = std::minmax_element(arr.begin(), arr.end());
    return (*p.second - *p.first);
}

//-------------------------------------------------------------------------------------------------
int argmin(const arr_real& arr) {
    return std::distance(arr.begin(), std::min_element(arr.begin(), arr.end()));
}

int argmin(const arr_cmplx& arr) {
    return std::distance(arr.begin(), std::min_element(arr.begin(), arr.end()));
}

//-------------------------------------------------------------------------------------------------
arr_real abs(const arr_real& arr) {
    arr_real r = arr;
    const int N = arr.size();
    for (int i = 0; i < N; ++i) {
        r[i] = std::fabs(arr[i]);
    }

    return r;
}

real_t abs(real_t v) {
    return std::fabs(v);
}

arr_real abs(const arr_cmplx& arr) {
    arr_real r(arr.size());
    const int N = arr.size();
    for (int i = 0; i < N; ++i) {
        r[i] = std::sqrt(arr[i].re * arr[i].re + arr[i].im * arr[i].im);
    }

    return r;
}

real_t abs(cmplx_t v) {
    return std::sqrt(v.re * v.re + v.im * v.im);
}

//-------------------------------------------------------------------------------------------------
arr_real fast_abs(const arr_cmplx& arr) {
    arr_real r(arr.size());
    const int N = arr.size();
    for (int i = 0; i < N; ++i) {
        r[i] = fast_abs(arr[i]);
    }

    return r;
}

real_t fast_abs(cmplx_t v) {
    real_t min, max;
    if (std::abs(v.re) > std::abs(v.im)) {
        min = std::abs(v.im);
        max = std::abs(v.re);
    } else {
        min = std::abs(v.re);
        max = std::abs(v.im);
    }

    return max + (min / 2);
}

//-------------------------------------------------------------------------------------------------
arr_real round(const arr_real& arr) {
    arr_real r(arr.size());
    const int N = arr.size();
    for (int i = 0; i < N; ++i) {
        r[i] = std::round(arr[i]);
    }

    return r;
}

arr_cmplx round(const arr_cmplx& arr) {
    arr_cmplx r(arr.size());
    const int N = arr.size();
    for (int i = 0; i < N; ++i) {
        r[i].re = std::round(arr[i].re);
        r[i].im = std::round(arr[i].im);
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
template<typename T>
static T _sum(const base_array<T>& x) {
    T acc = 0;
    const int n = x.size();
    for (int i = 0; i < n; ++i) {
        acc += x[i];
    }
    return acc;
}

real_t sum(const arr_real& arr) {
    return _sum(arr);
}

cmplx_t sum(const arr_cmplx& arr) {
    return _sum(arr);
}

//-------------------------------------------------------------------------------------------------
template<typename T>
static base_array<T> _cumsum(const base_array<T>& x, bool reverse) {
    base_array<T> r{x};
    const int n = x.size();
    if (!reverse) {
        for (int i = 1; i < n; ++i) {
            r[i] += r[i - 1];
        }
    } else {
        for (int i = n - 2; i >= 0; --i) {
            r[i] += r[i + 1];
        }
    }
    return r;
}

arr_real cumsum(const arr_real& x, Direction dir) {
    return _cumsum(x, dir == Direction::Reverse);
}

arr_cmplx cumsum(const arr_cmplx& x, Direction dir) {
    return _cumsum(x, dir == Direction::Reverse);
}

//-------------------------------------------------------------------------------------------------
std::pair<arr_real, arr_int> sort(const arr_real& x, Direction dir) {
    const int n = x.size();
    arr_int index{arange(n)};   //TODO: remove arr_real -> arr_int cast

    if (issorted(x, dir)) {
        return std::make_pair(x, index);
    }

    if (dir == Direction::Ascend) {
        std::sort(index.begin(), index.end(), [&](int i, int j) {
            return (x[i] < x[j]);
        });
    } else {
        std::sort(index.begin(), index.end(), [&](int i, int j) {
            return (x[i] > x[j]);
        });
    }
    const auto sorted = x[index];
    return std::make_pair(sorted, index);
}

bool issorted(const arr_real& x, Direction dir) {
    if (dir == Direction::Ascend) {
        return std::is_sorted(x.begin(), x.end());
    }
    if (dir == Direction::Descend) {
        auto comp = [&](auto lhs, auto rhs) {
            return (lhs > rhs);
        };
        return std::is_sorted(x.begin(), x.end(), comp);
    }
    return false;
}

//-------------------------------------------------------------------------------------------------
real_t dot(const arr_real& x1, const arr_real& x2) {
    if (x1.size() != x2.size()) {
        DSPLIB_THROW("array sizes are different");
    }
    real_t acc = 0;
    for (int i = 0; i < x1.size(); ++i) {
        acc += x1[i] * x2[i];
    }
    return acc;
}

cmplx_t dot(const arr_cmplx& x1, const arr_cmplx& x2) {
    if (x1.size() != x2.size()) {
        DSPLIB_THROW("array sizes are different");
    }
    cmplx_t acc = 0;
    for (int i = 0; i < x1.size(); ++i) {
        acc += x1[i] * x2[i];
    }
    return acc;
}

//-------------------------------------------------------------------------------------------------
real_t mean(const arr_real& arr) {
    real_t s = sum(arr);
    return s / arr.size();
}

cmplx_t mean(const arr_cmplx& arr) {
    cmplx_t s = sum(arr);
    return s / arr.size();
}

//-------------------------------------------------------------------------------------------------
real_t stddev(const arr_real& arr) {
    real_t m = mean(arr);
    return rms(arr - m);
}

real_t stddev(const arr_cmplx& arr) {
    auto m = mean(arr);
    return rms(arr - m);
}

//-------------------------------------------------------------------------------------------------
real_t median(const arr_real& arr) {
    arr_real r(arr);
    std::sort(r.begin(), r.end());
    const int n = r.size();
    return (n % 2 == 1) ? (r[n / 2]) : ((r[n / 2] + r[n / 2 - 1]) / 2);
}

//-------------------------------------------------------------------------------------------------
arr_real real(const arr_cmplx& x) {
    arr_real r(x.size());
    for (int i = 0; i < x.size(); ++i) {
        r[i] = x[i].re;
    }

    return r;
}

real_t real(cmplx_t x) {
    return x.re;
}

arr_real imag(const arr_cmplx& x) {
    arr_real r(x.size());
    for (int i = 0; i < x.size(); ++i) {
        r[i] = x[i].im;
    }

    return r;
}

real_t imag(cmplx_t x) {
    return x.im;
}

//-------------------------------------------------------------------------------------------------
int nextpow2(int m) {
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

    return p + 1;
}

bool ispow2(int m) {
    return (int(1) << nextpow2(m)) == m;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx complex(const arr_real& re, const arr_real& im) {
    if (re.size() != im.size()) {
        DSPLIB_THROW("array sizes are different");
    }

    int n = re.size();
    arr_cmplx r(n);
    for (int i = 0; i < n; ++i) {
        r[i].re = re[i];
        r[i].im = im[i];
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real log(const arr_real& arr) {
    int n = arr.size();
    arr_real r(n);
    for (int i = 0; i < n; ++i) {
        r[i] = std::log(arr[i]);
    }
    return r;
}

arr_real log2(const arr_real& arr) {
    int n = arr.size();
    arr_real r(n);
    for (int i = 0; i < n; ++i) {
        r[i] = std::log2(arr[i]);
    }
    return r;
}

arr_real log10(const arr_real& arr) {
    int n = arr.size();
    arr_real r(n);
    for (int i = 0; i < n; ++i) {
        r[i] = std::log10(arr[i]);
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
real_t log(const real_t& x) {
    return std::log(x);
}

real_t log2(const real_t& x) {
    return std::log2(x);
}

real_t log10(const real_t& x) {
    return std::log10(x);
}

//-------------------------------------------------------------------------------------------------
real_t rms(const arr_real& arr) {
    const int N = arr.size();
    real_t sum = 0;
    for (int i = 0; i < N; ++i) {
        sum += (arr[i] * arr[i]);
    }

    return std::sqrt(sum / (N - 1));
}

real_t rms(const arr_cmplx& arr) {
    const int N = arr.size();
    real_t sum = 0;
    for (int i = 0; i < N; ++i) {
        sum += (arr[i].re * arr[i].re);
        sum += (arr[i].im * arr[i].im);
    }

    return std::sqrt(sum / (N - 1));
}

//-------------------------------------------------------------------------------------------------
arr_real sin(const arr_real& arr) {
    int n = arr.size();
    arr_real r(n);
    for (int i = 0; i < n; ++i) {
        r[i] = std::sin(arr[i]);
    }
    return r;
}

arr_real cos(const arr_real& arr) {
    int n = arr.size();
    arr_real r(n);
    for (int i = 0; i < n; ++i) {
        r[i] = std::cos(arr[i]);
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx conj(const arr_cmplx& x) {
    int n = x.size();
    arr_cmplx r = x;
    for (int i = 0; i < n; ++i) {
        r[i].im = -r[i].im;
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real pow2(const arr_real& arr) {
    arr_real r(arr);
    for (int i = 0; i < arr.size(); ++i) {
        r[i] *= r[i];
    }

    return r;
}

arr_cmplx pow2(const arr_cmplx& arr) {
    arr_cmplx r(arr);
    for (int i = 0; i < arr.size(); ++i) {
        r[i] *= r[i];
    }

    return r;
}

real_t pow(real_t x, real_t n) {
    return std::pow(x, n);
}

cmplx_t pow(cmplx_t x, real_t n) {
    real_t x_p = angle(x);
    real_t x_a = abs(x);
    return pow(x_a, n) * expj(x_p * n);
}

arr_cmplx pow(cmplx_t x, const arr_real& n) {
    arr_cmplx r(n.size());
    const real_t x_p = angle(x);
    const real_t x_a = abs(x);
    for (int i = 0; i < n.size(); ++i) {
        r[i] = pow(x_a, n[i]) * expj(x_p * n[i]);
    }
    return r;
}

arr_real pow(real_t x, const arr_real& n) {
    arr_real r(n.size());
    for (int i = 0; i < n.size(); ++i) {
        r[i] = pow(x, n[i]);
    }
    return r;
}

template<typename T1, typename T2, typename R>
static base_array<R> _pow(const base_array<T1>& x, const base_array<T2>& n) {
    if (x.size() != n.size()) {
        DSPLIB_THROW("array sizes are different");
    }
    base_array<R> r(n.size());
    for (int i = 0; i < n.size(); ++i) {
        r[i] = pow(x[i], n[i]);
    }
    return r;
}

arr_real pow(const arr_real& x, const arr_real& n) {
    return _pow<real_t, real_t, real_t>(x, n);
}

arr_cmplx pow(const arr_cmplx& x, const arr_real& n) {
    return _pow<cmplx_t, real_t, cmplx_t>(x, n);
}

template<typename T>
static base_array<T> _pow(const base_array<T>& x, real_t n) {
    base_array<T> r(x.size());
    for (int i = 0; i < x.size(); ++i) {
        r[i] = pow(x[i], n);
    }
    return r;
}

arr_real pow(const arr_real& x, real_t n) {
    return _pow(x, n);
}

arr_cmplx pow(const arr_cmplx& x, real_t n) {
    return _pow(x, n);
}

//-------------------------------------------------------------------------------------------------
arr_real angle(const arr_cmplx& arr) {
    arr_real r(arr.size());
    for (int i = 0; i < r.size(); ++i) {
        r[i] = angle(arr[i]);
    }

    return r;
}

real_t angle(cmplx_t v) {
    real_t d = 0;
    if (v.re < 0) {
        d = (v.im > 0) ? (pi) : (-pi);
    } else {
        d = 0;
    }

    return std::atan(v.im / v.re) + d;
}

//-------------------------------------------------------------------------------------------------
arr_real exp(const arr_real& arr) {
    arr_real r(arr);
    for (int i = 0; i < r.size(); ++i) {
        r[i] = std::exp(r[i]);
    }
    return r;
}

real_t exp(real_t v) {
    return std::exp(v);
}

arr_cmplx exp(const arr_cmplx& arr) {
    arr_cmplx r(arr);
    real_t v;
    for (int i = 0; i < r.size(); ++i) {
        v = std::exp(r[i].re);
        r[i].re = v * std::cos(r[i].im);
        r[i].im = v * std::sin(r[i].im);
    }

    return r;
}

cmplx_t exp(cmplx_t v) {
    return cmplx_t{std::exp(v.re) * std::cos(v.im), std::exp(v.re) * std::sin(v.im)};
}

arr_cmplx expj(const arr_real& im) {
    arr_cmplx r(im.size());
    for (int i = 0; i < r.size(); ++i) {
        r[i].re = std::cos(im[i]);
        r[i].im = std::sin(im[i]);
    }

    return r;
}

cmplx_t expj(real_t im) {
    return cmplx_t{std::cos(im), std::sin(im)};
}

//-------------------------------------------------------------------------------------------------
arr_real tanh(arr_real x) {
    for (int i = 0; i < x.size(); ++i) {
        x[i] = std::tanh(x[i]);
    }
    return x;
}

arr_cmplx tanh(arr_cmplx x) {
    for (int i = 0; i < x.size(); ++i) {
        x[i] = std::tanh(std::complex<real_t>(x[i]));
    }
    return x;
}

//-------------------------------------------------------------------------------------------------
template<class T>
static T _downsample(const T& arr, int n, int phase) {
    if (n <= 0) {
        DSPLIB_THROW("downsample factor must be greater 0");
    }

    if (phase >= n || phase < 0) {
        DSPLIB_THROW("phase must be [0, N-1]");
    }

    if (n == 1) {
        return arr;
    }

    const int nr = (arr.size() - phase - 1) / n + 1;
    T r(nr);
    for (size_t i = 0, k = phase; k < arr.size(); ++i, k += n) {
        r[i] = arr[k];
    }
    return r;
}

arr_real downsample(const arr_real& arr, int n, int phase) {
    return _downsample(arr, n, phase);
}

arr_cmplx downsample(const arr_cmplx& arr, int n, int phase) {
    return _downsample(arr, n, phase);
}

//-------------------------------------------------------------------------------------------------
template<class T>
static T _upsample(const T& arr, int n, int phase) {
    if (n <= 0) {
        DSPLIB_THROW("upsample factor must be greater 0");
    }

    if (phase >= n || phase < 0) {
        DSPLIB_THROW("phase must be [0, N-1]");
    }

    if (n == 1) {
        return arr;
    }

    T r(arr.size() * n);
    for (size_t i = 0, k = phase; k < r.size(); ++i, k += n) {
        r[k] = arr[i];
    }
    return r;
}

arr_real upsample(const arr_real& arr, int n, int phase) {
    return _upsample(arr, n, phase);
}

arr_cmplx upsample(const arr_cmplx& arr, int n, int phase) {
    return _upsample(arr, n, phase);
}

//-------------------------------------------------------------------------------------------------
arr_real abs2(const arr_cmplx& x) {
    arr_real r(x.size());
    for (size_t i = 0; i < x.size(); i++) {
        r[i] = (x[i].re * x[i].re) + (x[i].im * x[i].im);
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
real_t norm(const arr_real& x, int p) {
    if (p == 1) {
        return sum(abs(x));
    }
    if (p == 2) {
        return sqrt(sum(pow2(x)));
    }
    return pow(sum(pow(abs(x), p)), 1.0 / p);
}

real_t norm(const arr_cmplx& x, int p) {
    if (p == 1) {
        return sum(abs(x));
    }
    if (p == 2) {
        return sqrt(sum(abs2(x)));
    }
    return pow(sum(pow(abs(x), p)), 1.0 / p);
}

//-------------------------------------------------------------------------------------------------
arr_real deg2rad(const arr_real& x) {
    return x / 180.0 * pi;
}

real_t deg2rad(const real_t& x) {
    return x / 180.0 * pi;
}

//-------------------------------------------------------------------------------------------------
arr_real rad2deg(const arr_real& x) {
    return x / pi * 180;
}

real_t rad2deg(const real_t& x) {
    return x / pi * 180;
}

//-------------------------------------------------------------------------------------------------
real_t pow2db(real_t v) {
    return 10 * std::log10(v);
}

arr_real pow2db(const arr_real& v) {
    return 10 * log10(v);
}

real_t db2pow(real_t v) {
    return std::pow(real_t(10), (v / 10));
}

arr_real db2pow(const arr_real& v) {
    return pow(10, (v / 10));
}

//-------------------------------------------------------------------------------------------------
real_t mag2db(real_t v) {
    return 20 * std::log10(v);
}

arr_real mag2db(const arr_real& v) {
    return 20 * log10(v);
}

real_t db2mag(real_t v) {
    return std::pow(real_t(10), (v / 20));
}

arr_real db2mag(const arr_real& v) {
    return pow(10, (v / 20));
}

}   // namespace dsplib
