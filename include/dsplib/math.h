#pragma once

#include <dsplib/array.h>
#include <limits>

namespace dsplib {

//exponential
arr_real exp(const arr_real& arr);
real_t exp(real_t v);
arr_cmplx exp(const arr_cmplx& arr);
cmplx_t exp(cmplx_t v);

//complex exponential for zero Re and non-zero Im
//faster then exp()
arr_cmplx expj(const arr_real& im);
cmplx_t expj(real_t im);

//max element
real_t max(const arr_real& arr);
cmplx_t max(const arr_cmplx& arr);

template<typename T1, typename T2>
auto max(const T1& v1, const T2& v2) -> decltype(v1 + v2) {
    return (v1 > v2) ? (v1) : (v2);
}

//min element
real_t min(const arr_real& arr);
cmplx_t min(const arr_cmplx& arr);

template<typename T1, typename T2>
auto min(const T1& v1, const T2& v2) -> decltype(v1 + v2) {
    return (v1 < v2) ? (v1) : (v2);
}

//max element index
int argmax(const arr_real& arr);
int argmax(const arr_cmplx& arr);

//min element index
int argmin(const arr_real& arr);
int argmin(const arr_cmplx& arr);

//absolute value and complex magnitude
arr_real abs(const arr_real& arr);
real_t abs(real_t v);
arr_real abs(const arr_cmplx& arr);
real_t abs(cmplx_t v);

//phase angle in the interval [-pi, pi] for each element of a complex array z
arr_real angle(const arr_cmplx& arr);
real_t angle(cmplx_t v);

//fast absolute complex magnitude
arr_real fabs(const arr_cmplx& arr);
real_t fabs(cmplx_t v);

//round
arr_real round(const arr_real& arr);
arr_cmplx round(const arr_cmplx& arr);

//array sum
real_t sum(const arr_real& arr);
cmplx_t sum(const arr_cmplx& arr);

//array mean
real_t mean(const arr_real& arr);
cmplx_t mean(const arr_cmplx& arr);

//standard deviation
real_t stddev(const arr_real& arr);
real_t stddev(const arr_cmplx& arr);

//median
real_t median(const arr_real& arr);

//real part
arr_real real(const arr_cmplx& x);
real_t real(cmplx_t x);

//imag part
arr_real imag(const arr_cmplx& x);
real_t imag(cmplx_t x);

//complex pairing
arr_cmplx conj(const arr_cmplx& x);
cmplx_t conj(cmplx_t x);

inline arr_real conj(const arr_real& x) {
    return x;
};
inline real_t conj(real_t x) {
    return x;
};

//complex vector formation
arr_cmplx complex(const arr_real& re, const arr_real& im);

//the nearest power of two numbers (with rounding up)
int nextpow2(int m);

//array pow
arr_real pow2(const arr_real& arr);
arr_cmplx pow2(const arr_cmplx& arr);

inline real_t pow2(real_t x) {
    return x * x;
}
inline cmplx_t pow2(cmplx_t x) {
    return x * x;
}

real_t pow(real_t x, real_t n);
cmplx_t pow(cmplx_t x, real_t n);

//array log
arr_real log(const arr_real& arr);
arr_real log2(const arr_real& arr);
arr_real log10(const arr_real& arr);

real_t log(const real_t& x);
real_t log2(const real_t& x);
real_t log10(const real_t& x);

//array rms
real_t rms(const arr_real& arr);
real_t rms(const arr_cmplx& arr);

//trigonometric functions
arr_real sin(const arr_real& arr);
arr_real cos(const arr_real& arr);

//integer uniform [1, imax]
int randi(int imax);
arr_int randi(int imax, int n);

//integer uniform [imin, imax]
int randi(std::array<int, 2> range);
arr_int randi(std::array<int, 2> range, int n);

//uniformly distributed random numbers
real_t rand();
arr_real rand(int n);

//normally distributed random numbers
real_t randn();
arr_real randn(int n);

//decrease sample rate by integer factor
arr_real downsample(const arr_real& arr, int n, int phase = 0);
arr_cmplx downsample(const arr_cmplx& arr, int n, int phase = 0);

//abs(x)^2
arr_real abs2(const arr_cmplx& x);
real_t abs2(const cmplx_t& x);

inline arr_real abs2(const arr_real& x) {
    return pow2(x);
}
inline real_t abs2(const real_t& x) {
    return pow2(x);
}

//from degrees to radians
arr_real deg2rad(const arr_real& x);
real_t deg2rad(const real_t& x);

//from radians to degrees
arr_real rad2deg(const arr_real& x);
real_t rad2deg(const real_t& x);

constexpr real_t eps() {
    return std::numeric_limits<real_t>::epsilon();
}

//Euclidean norm of vector
inline real_t norm(const arr_real& x) {
    return sqrt(sum(pow2(x)));
}
inline real_t norm(const arr_cmplx& x) {
    return sqrt(sum(abs2(x)));
}

//Mean squared error
inline real_t mse(const arr_real& x, const arr_real& y) {
    return mean(pow2(x - y));
}
inline real_t mse(const arr_cmplx& x, const arr_cmplx& y) {
    return mean(abs2(x - y));
}

//Normalized mean squared error
inline real_t nmse(const arr_real& x, const arr_real& y) {
    return mse(x, y) / sum(pow2(x));
}
inline real_t nmse(const arr_cmplx& x, const arr_cmplx& y) {
    return mse(x, y) / sum(abs2(x));
}

//----------------------------------------------------------------------------------------
//pow(scalar, vec) -> vec
inline arr_real operator^(const real_t& v, const arr_real& rhs) {
    arr_real res = rhs;
    for (size_t i = 0; i < rhs.size(); i++) {
        res[i] = pow(v, rhs[i]);
    }
    return res;
}

inline arr_cmplx operator^(const cmplx_t& v, const arr_real& rhs) {
    arr_cmplx res = rhs;
    for (size_t i = 0; i < rhs.size(); i++) {
        res[i] = pow(v, rhs[i]);
    }
    return res;
}

}   // namespace dsplib
