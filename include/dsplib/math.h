#pragma once

#include <dsplib/array.h>
#include <dsplib/keywords.h>

#include <cstdint>

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

//hyperbolic tangent
arr_real tanh(arr_real x);
arr_cmplx tanh(arr_cmplx x);

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

// range of values (maximum - minimum)
real_t peak2peak(const arr_real& arr);
cmplx_t peak2peak(const arr_cmplx& arr);

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

//round
real_t round(const real_t& x);
cmplx_t round(const cmplx_t& x);
arr_real round(const arr_real& arr);
arr_cmplx round(const arr_cmplx& arr);

//array sum
real_t sum(const arr_real& arr);
cmplx_t sum(const arr_cmplx& arr);
int sum(const std::vector<bool>& arr);

// cumulative sum
// example: cumsum([1, 2, 3, 4, 5]) -> [1, 3, 6, 10, 15]
arr_real cumsum(const arr_real& x, Direction dir = Direction::Forward);
arr_cmplx cumsum(const arr_cmplx& x, Direction dir = Direction::Forward);

//array dot
real_t dot(const arr_real& x1, const arr_real& x2);
cmplx_t dot(const arr_cmplx& x1, const arr_cmplx& x2);

//array mean
real_t mean(const arr_real& arr);
cmplx_t mean(const arr_cmplx& arr);

//standard deviation
real_t stddev(const arr_real& arr);
real_t stddev(const arr_cmplx& arr);

//median
real_t median(const arr_real& arr);

//linear or rank correlation
//TODO: add p-value result
real_t corr(const arr_real& x, const arr_real& y, Correlation type = Correlation::Pearson);

//sort array elements
//result: [sorted array, sort index]
std::pair<arr_real, arr_int> sort(const arr_real& x, Direction dir = Direction::Ascend);

//determine if array is sorted
bool issorted(const arr_real& x, Direction dir = Direction::Ascend);

//real part
arr_real real(const arr_cmplx& x);
real_t real(cmplx_t x);

//imag part
arr_real imag(const arr_cmplx& x);
real_t imag(cmplx_t x);

//complex pairing
arr_cmplx conj(const arr_cmplx& x);

constexpr cmplx_t conj(cmplx_t x) {
    return x.conj();
}

inline arr_real conj(const arr_real& x) {
    return x;
};

constexpr real_t conj(real_t x) {
    return x;
};

//complex vector formation
arr_cmplx complex(const arr_real& re, const arr_real& im);
arr_cmplx complex(const arr_real& re) noexcept;

//the nearest power of two numbers (with rounding up)
int nextpow2(int m);

//checks if m is an integral power of two
bool ispow2(int m);

//scalar^scalar->scalar
real_t power(real_t x, real_t n);
cmplx_t power(cmplx_t x, real_t n);

//scalar^vec->vec
arr_cmplx power(cmplx_t x, const arr_real& n);

//scalar^vec->vec
arr_real power(real_t x, const arr_real& n);

//vec.^vec->vec
arr_real power(const arr_real& x, const arr_real& n);
arr_cmplx power(const arr_cmplx& x, const arr_real& n);

//vec.^scalar->vec
arr_real power(const arr_real& x, real_t n);
arr_cmplx power(const arr_cmplx& x, real_t n);

//integer power
real_t power(real_t x, int n);
cmplx_t power(cmplx_t x, int n);
arr_real power(const arr_real& x, int n);
arr_cmplx power(const arr_cmplx& x, int n);

//square root (only positive values)
//TODO: add complex result for negative or complex input
real_t sqrt(real_t x);
arr_real sqrt(const arr_real& x);

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

//decrease sample rate by integer factor
arr_real downsample(const arr_real& arr, int n, int phase = 0);
arr_cmplx downsample(const arr_cmplx& arr, int n, int phase = 0);

//increase sample rate by integer factor
arr_real upsample(const arr_real& arr, int n, int phase = 0);
arr_cmplx upsample(const arr_cmplx& arr, int n, int phase = 0);

//abs(x)^2
arr_real abs2(const arr_cmplx& x);

constexpr real_t abs2(const cmplx_t& x) {
    return x.abs2();
}

inline arr_real abs2(const arr_real& x) {
    return power(x, 2);
}

constexpr real_t abs2(const real_t& x) {
    return (x * x);
}

//from degrees to radians
arr_real deg2rad(const arr_real& x);
real_t deg2rad(const real_t& x);

//from radians to degrees
arr_real rad2deg(const arr_real& x);
real_t rad2deg(const real_t& x);

//vector norms
//p=1, sum(abs(x))
//p=2, euclidean norm of vector, sum(abs(x).^2)^(1/2)
//p>0, sum(abs(x).^p)^(1/p)
real_t norm(const arr_real& x, int p = 2);
real_t norm(const arr_cmplx& x, int p = 2);

//Mean squared error
inline real_t mse(const arr_real& x, const arr_real& y) {
    return mean(abs2(x - y));
}
inline real_t mse(const arr_cmplx& x, const arr_cmplx& y) {
    return mean(abs2(x - y));
}

//Normalized mean squared error
inline real_t nmse(const arr_real& x, const arr_real& y) {
    return mse(x, y) / sum(abs2(x));
}
inline real_t nmse(const arr_cmplx& x, const arr_cmplx& y) {
    return mse(x, y) / sum(abs2(x));
}

//signum function
constexpr int sign(const real_t& x) {
    return (real_t(0) < x) - (x < real_t(0));
}

inline cmplx_t sign(const cmplx_t& x) {
    if (x.re == 0 && x.im == 0) {
        return 0;
    }
    return x / abs(x);
}

//----------------------------------------------------------------------------------------
//convert power <-> decibels: db = 10 * log10(pow)
real_t pow2db(real_t v);
arr_real pow2db(const arr_real& v);
real_t db2pow(real_t v);
arr_real db2pow(const arr_real& v);

//convert magnitude <-> decibels: db = 20 * log10(mag)
real_t mag2db(real_t v);
arr_real mag2db(const arr_real& v);
real_t db2mag(real_t v);
arr_real db2mag(const arr_real& v);

//----------------------------------------------------------------------------------------
//check that the number is prime
//example: isprime(5) = true
bool isprime(uint32_t n) noexcept;

//prime factors
//example: factor(10) = {2, 5}
arr_int factor(uint32_t n);

//next prime number
//example: nextprime(10) == 11
//example: nextprime(257) == 257
uint32_t nextprime(uint32_t n);

//prime numbers (less than or equal)
//example: primes(11) = {2, 3, 5, 7, 11}
//example: primes(10) = {2, 3, 5, 7}
arr_int primes(uint32_t n);

//----------------------------------------------------------------------------------------
//missing data

//determine if any array element is NaN
bool anynan(const arr_real& x);
bool anynan(const arr_cmplx& x);

//determine if any array element is Inf or -Inf
bool anyinf(const arr_real& x);
bool anyinf(const arr_cmplx& x);

}   // namespace dsplib
