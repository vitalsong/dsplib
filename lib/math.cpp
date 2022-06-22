#include <dsplib/math.h>

#include <stdexcept>
#include <algorithm>
#include <random>
#include <cmath>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
real_t max(const arr_real& arr) {
    return *std::max_element(arr.begin(), arr.end());
}

//-------------------------------------------------------------------------------------------------
cmplx_t max(const arr_cmplx& arr) {
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
real_t min(const arr_real& arr) {
    return *std::min_element(arr.begin(), arr.end());
}

//-------------------------------------------------------------------------------------------------
cmplx_t min(const arr_cmplx& arr) {
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
arr_real abs(const arr_real& arr) {
    arr_real r = arr;
    const int N = arr.size();
    for (int i = 0; i < N; ++i) {
        r[i] = std::fabs(arr[i]);
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
real_t abs(real_t v) {
    return std::fabs(v);
}

//-------------------------------------------------------------------------------------------------
arr_real abs(const arr_cmplx& arr) {
    arr_real r(arr.size());
    const int N = arr.size();
    for (int i = 0; i < N; ++i) {
        r[i] = std::sqrt(arr[i].re * arr[i].re + arr[i].im * arr[i].im);
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
real_t abs(cmplx_t v) {
    return std::sqrt(v.re * v.re + v.im * v.im);
}

//-------------------------------------------------------------------------------------------------
arr_real fabs(const arr_cmplx& arr) {
    arr_real r(arr.size());
    const int N = arr.size();
    for (int i = 0; i < N; ++i) {
        r[i] = fabs(arr[i]);
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
real_t fabs(cmplx_t v) {
    real_t min, max;
    if (std::fabs(v.re) > std::fabs(v.im)) {
        min = std::fabs(v.im);
        max = std::fabs(v.re);
    } else {
        min = std::fabs(v.re);
        max = std::fabs(v.im);
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

//-------------------------------------------------------------------------------------------------
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
real_t sum(const arr_real& arr) {
    real_t acc = 0;
    const int N = arr.size();
    for (int i = 0; i < N; ++i) {
        acc += arr[i];
    }

    return acc;
}

//-------------------------------------------------------------------------------------------------
cmplx_t sum(const arr_cmplx& arr) {
    cmplx_t acc = 0;
    const int N = arr.size();
    for (int i = 0; i < N; ++i) {
        acc.re += arr[i].re;
        acc.im += arr[i].im;
    }

    return acc;
}

//-------------------------------------------------------------------------------------------------
real_t mean(const arr_real& arr) {
    real_t s = sum(arr);
    return s / arr.size();
}

//-------------------------------------------------------------------------------------------------
cmplx_t mean(const arr_cmplx& arr) {
    cmplx_t s = sum(arr);
    return s / arr.size();
}

//-------------------------------------------------------------------------------------------------
real_t stddev(const arr_real& arr) {
    real_t m = mean(arr);
    return rms(arr - m);
}

//-------------------------------------------------------------------------------------------------
real_t stddev(const arr_cmplx& arr) {
    auto m = mean(arr);
    return rms(arr - m);
}

//-------------------------------------------------------------------------------------------------
real_t median(const arr_real& arr) {
    arr_real r(arr);
    std::sort(r.begin(), r.end());
    return r[r.size() / 2];
}

//-------------------------------------------------------------------------------------------------
arr_real real(const arr_cmplx& x) {
    arr_real r(x.size());
    for (int i = 0; i < x.size(); ++i) {
        r[i] = x[i].re;
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
real_t real(cmplx_t x) {
    return x.re;
}

//-------------------------------------------------------------------------------------------------
arr_real imag(const arr_cmplx& x) {
    arr_real r(x.size());
    for (int i = 0; i < x.size(); ++i) {
        r[i] = x[i].im;
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
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
    } else {
        return p + 1;
    }
}

//-------------------------------------------------------------------------------------------------
arr_cmplx complex(const arr_real& re, const arr_real& im) {
    if (re.size() != im.size()) {
        throw std::invalid_argument("array sizes are different");
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

//-------------------------------------------------------------------------------------------------
arr_real log2(const arr_real& arr) {
    int n = arr.size();
    arr_real r(n);
    for (int i = 0; i < n; ++i) {
        r[i] = std::log2(arr[i]);
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------
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
cmplx_t conj(cmplx_t x) {
    x.im = -x.im;
    return x;
}

//-------------------------------------------------------------------------------------------------
arr_real pow2(const arr_real& arr) {
    arr_real r(arr);
    for (int i = 0; i < arr.size(); ++i) {
        r[i] *= r[i];
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx pow2(const arr_cmplx& arr) {
    arr_cmplx r(arr);
    for (int i = 0; i < arr.size(); ++i) {
        r[i] *= r[i];
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
real_t pow(real_t x, real_t n) {
    return std::pow(x, n);
}

//-------------------------------------------------------------------------------------------------
cmplx_t pow(cmplx_t x, real_t n) {
    real_t x_p = angle(x);
    real_t x_a = abs(x);
    return std::pow(x_a, n) * expj(x_p * n);
}

//-------------------------------------------------------------------------------------------------
arr_real angle(const arr_cmplx& arr) {
    arr_real r(arr.size());
    for (int i = 0; i < r.size(); ++i) {
        r[i] = angle(arr[i]);
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
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
arr_real randn(int n) {
    arr_real r(n);
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<real_t> d(0, 1);
    for (int i = 0; i < n; ++i) {
        r[i] = d(gen);
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
real_t randn() {
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<real_t> d(0, 1);
    return d(gen);
}

//-------------------------------------------------------------------------------------------------
arr_real exp(const arr_real& arr) {
    arr_real r(arr);
    for (int i = 0; i < r.size(); ++i) {
        r[i] = std::exp(r[i]);
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
real_t exp(real_t v) {
    return std::exp(v);
}

//-------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------
cmplx_t exp(cmplx_t v) {
    return cmplx_t{std::exp(v.re) * std::cos(v.im), std::exp(v.re) * std::sin(v.im)};
}

//-------------------------------------------------------------------------------------------------
arr_cmplx expj(const arr_real& im) {
    arr_cmplx r(im.size());
    for (int i = 0; i < r.size(); ++i) {
        r[i].re = std::cos(im[i]);
        r[i].im = std::sin(im[i]);
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
cmplx_t expj(real_t im) {
    return cmplx_t{std::cos(im), std::sin(im)};
}

//-------------------------------------------------------------------------------------------------
template<class T>
static T _downsample(const T& arr, int n, int phase = 0) {
    if (n == 0) {
        throw std::invalid_argument("downsample factor must be greater 0");
    }

    T r;
    for (size_t i = phase; i < arr.size(); i += n) {
        r.push_back(arr[i]);
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real downsample(const arr_real& arr, int n, int phase) {
    return _downsample(arr, n, phase);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx downsample(const arr_cmplx& arr, int n, int phase) {
    return _downsample(arr, n, phase);
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
real_t abs2(const cmplx_t& x) {
    return (x.re * x.re) + (x.im * x.im);
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

}   // namespace dsplib
