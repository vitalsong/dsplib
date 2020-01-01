#ifndef MATH_H
#define MATH_H

#include <dsplib/array.h>

namespace dsplib {

//max element
real_t max(const arr_real& arr);
cmplx_t max(const arr_cmplx& arr);

//min element
real_t min(const arr_real& arr);
cmplx_t min(const arr_cmplx& arr);

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

//array std
real_t stddev(const arr_real& arr);

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

//array zeroing
void zeroing(arr_cmplx& arr);
void zeroing(arr_real& arr);

//the nearest power of two numbers (with rounding up)
int nextpow2(int m);

//array pow
arr_real pow2(const arr_real& arr);
arr_cmplx pow2(const arr_cmplx& arr);

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

//approximation of the index correction (-1: 1) by a parabola at three points
real_t apprx(real_t y0, real_t y1, real_t y2);

} ///< dsplib

#endif // MATH_H
