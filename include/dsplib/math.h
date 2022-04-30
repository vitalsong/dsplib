#pragma once

#include <dsplib/array.h>

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

//complex vector formation
arr_cmplx complex(const arr_real& re, const arr_real& im);

//the nearest power of two numbers (with rounding up)
int nextpow2(int m);

//array pow
arr_real pow2(const arr_real& arr);
arr_cmplx pow2(const arr_cmplx& arr);
real_t pow(real_t x, real_t n);
cmplx_t pow(cmplx_t x, real_t n);

//array log
arr_real log(const arr_real& arr);
arr_real log2(const arr_real& arr);
arr_real log10(const arr_real& arr);

//array rms
real_t rms(const arr_real& arr);
real_t rms(const arr_cmplx& arr);

//trigonometric functions
arr_real sin(const arr_real& arr);
arr_real cos(const arr_real& arr);

//normally distributed random numbers
real_t randn();
arr_real randn(int n);

//decrease sample rate by integer factor
arr_real downsample(const arr_real& arr, int n, int phase = 0);
arr_cmplx downsample(const arr_cmplx& arr, int n, int phase = 0);

//abs(x)^2
arr_real abs2(const arr_cmplx& x);
real_t abs2(const cmplx_t& x);

//from degrees to radians
arr_real deg2rad(const arr_real& x);
real_t deg2rad(const real_t& x);

//from radians to degrees
arr_real rad2deg(const arr_real& x);
real_t rad2deg(const real_t& x);

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
