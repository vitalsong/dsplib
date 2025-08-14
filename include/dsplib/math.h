#pragma once

#include <dsplib/array.h>
#include <dsplib/keywords.h>

#include <cstdint>

namespace dsplib {

//TODO: mark noexcept

//exponential
arr_real exp(span_real arr);
real_t exp(real_t v);
arr_cmplx exp(span_cmplx arr);
cmplx_t exp(cmplx_t v);

//complex exponential for zero Re and non-zero Im
//faster then exp()
arr_cmplx expj(span_real w);
cmplx_t expj(real_t w);

//hyperbolic tangent
arr_real tanh(span_real x);
arr_cmplx tanh(span_cmplx x);

//max element
real_t max(span_real arr);
cmplx_t max(span_cmplx arr);

template<typename T1, typename T2>
auto max(const T1& v1, const T2& v2) -> decltype(v1 + v2) {
    return (v1 > v2) ? (v1) : (v2);
}

//min element
real_t min(span_real arr);
cmplx_t min(span_cmplx arr);

template<typename T1, typename T2>
auto min(const T1& v1, const T2& v2) -> decltype(v1 + v2) {
    return (v1 < v2) ? (v1) : (v2);
}

// range of values (maximum - minimum)
real_t peak2peak(span_real arr);
cmplx_t peak2peak(span_cmplx arr);

//max element index
int argmax(span_real arr);
int argmax(span_cmplx arr);

//min element index
int argmin(span_real arr);
int argmin(span_cmplx arr);

//absolute value and complex magnitude
arr_real abs(span_real arr) noexcept;
real_t abs(real_t v) noexcept;
arr_real abs(span_cmplx arr) noexcept;
real_t abs(cmplx_t v) noexcept;
void abs(inplace_real arr) noexcept;

//phase angle in the interval [-pi, pi] for each element of a complex array z
arr_real angle(span_cmplx arr);
real_t angle(cmplx_t v);

//round
real_t round(const real_t& x) noexcept;
cmplx_t round(const cmplx_t& x) noexcept;
arr_real round(span_real arr) noexcept;
arr_cmplx round(span_cmplx arr) noexcept;
void round(inplace_real arr) noexcept;
void round(inplace_cmplx arr) noexcept;

//array sum
real_t sum(span_real arr);
cmplx_t sum(span_cmplx arr);
int sum(const std::vector<bool>& arr);

// cumulative sum
// example: cumsum([1, 2, 3, 4, 5]) -> [1, 3, 6, 10, 15]
arr_real cumsum(span_real x, Direction dir = Direction::Forward);
arr_cmplx cumsum(span_cmplx x, Direction dir = Direction::Forward);

//array dot
real_t dot(span_real x1, span_real x2);
cmplx_t dot(span_cmplx x1, span_cmplx x2);

//array mean
real_t mean(span_real arr);
cmplx_t mean(span_cmplx arr);

//standard deviation
real_t stddev(span_real arr);
real_t stddev(span_cmplx arr);

//median
real_t median(span_real arr);

//linear or rank correlation
//TODO: add p-value result
real_t corr(span_real x, span_real y, Correlation type = Correlation::Pearson);

//sort array elements
//result: [sorted array, sort index]
std::pair<arr_real, arr_int> sort(const arr_real& x, Direction dir = Direction::Ascend);

//determine if array is sorted
bool issorted(span_real x, Direction dir = Direction::Ascend);

//real part
arr_real real(span_cmplx x);
real_t real(const cmplx_t& x);

//imag part
arr_real imag(span_cmplx x);
real_t imag(const cmplx_t& x);

//complex pairing
arr_cmplx conj(const arr_cmplx& x) noexcept;

void conj(inplace_cmplx x) noexcept;

constexpr cmplx_t conj(const cmplx_t& x) noexcept {
    return x.conj();
}

inline arr_real conj(const arr_real& x) noexcept {
    return x;
};

constexpr real_t conj(const real_t& x) noexcept {
    return x;
};

//complex vector formation
arr_cmplx complex(span_real re, span_real im);
arr_cmplx complex(span_real re);

//the nearest power of two numbers (with rounding up)
constexpr int nextpow2(int m) noexcept {
    assert(m >= 0);

    if ((m == 0) || (m == 1)) {
        return 0;
    }

    //TODO: if pow2 - use __builtin_ctz

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

//checks if m is an integral power of two
constexpr bool ispow2(int m) noexcept {
    assert(m >= 0);
    return (m > 0) && (m & (m - 1)) == 0;
}

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
real_t sqrt(real_t x) noexcept;
arr_real sqrt(span_real arr) noexcept;
void sqrt(inplace_real arr) noexcept;

//array log
arr_real log(span_real arr);
arr_real log2(span_real arr);
arr_real log10(span_real arr);

real_t log(const real_t& x);
real_t log2(const real_t& x);
real_t log10(const real_t& x);

//array rms
real_t rms(span_real arr);
real_t rms(span_cmplx arr);

//trigonometric functions
arr_real sin(span_real arr);
arr_real cos(span_real arr);

//decrease sample rate by integer factor
arr_real downsample(span_real arr, int n, int phase = 0);
arr_cmplx downsample(span_cmplx arr, int n, int phase = 0);

//increase sample rate by integer factor
arr_real upsample(span_real arr, int n, int phase = 0);
arr_cmplx upsample(span_cmplx arr, int n, int phase = 0);

//abs(x)^2
arr_real abs2(const arr_cmplx& x) noexcept;

constexpr real_t abs2(const cmplx_t& x) noexcept {
    return x.abs2();
}

inline arr_real abs2(const arr_real& x) {
    return power(x, 2);
}

constexpr real_t abs2(const real_t& x) noexcept {
    return (x * x);
}

//from degrees to radians
arr_real deg2rad(span_real x);
real_t deg2rad(const real_t& x);

//from radians to degrees
arr_real rad2deg(span_real x);
real_t rad2deg(const real_t& x);

//vector norms
//p=1, sum(abs(x))
//p=2, euclidean norm of vector, sum(abs(x).^2)^(1/2)
//p>0, sum(abs(x).^p)^(1/p)
real_t norm(span_real x, int p = 2);
real_t norm(span_cmplx x, int p = 2);

//Mean squared error
real_t mse(span_real x, span_real y);
real_t mse(span_cmplx x, span_cmplx y);

//Normalized mean squared error
real_t nmse(span_real x, span_real y);
real_t nmse(span_cmplx x, span_cmplx y);

//signum function
constexpr int sign(const real_t& x) noexcept {
    return (real_t(0) < x) - (x < real_t(0));
}

inline cmplx_t sign(const cmplx_t& x) noexcept {
    if (x.re == 0 && x.im == 0) {
        return 0;
    }
    return x / abs(x);
}

//----------------------------------------------------------------------------------------
//convert power <-> decibels: db = 10 * log10(pow)
real_t pow2db(real_t v) noexcept;
arr_real pow2db(span_real arr) noexcept;
real_t db2pow(real_t v) noexcept;
arr_real db2pow(span_real arr) noexcept;
void pow2db(inplace_real arr) noexcept;
void db2pow(inplace_real arr) noexcept;

//convert magnitude <-> decibels: db = 20 * log10(mag)
real_t mag2db(real_t v) noexcept;
arr_real mag2db(span_real arr) noexcept;
real_t db2mag(real_t v) noexcept;
arr_real db2mag(span_real arr) noexcept;
void mag2db(inplace_real arr) noexcept;
void db2mag(inplace_real arr) noexcept;

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
bool anynan(span_real x);
bool anynan(span_cmplx x);

//determine if any array element is Inf or -Inf
bool anyinf(span_real x);
bool anyinf(span_cmplx x);

}   // namespace dsplib
