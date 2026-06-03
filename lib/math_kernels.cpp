// This translation unit may be compiled with unsafe floating-point optimizations
// when DSPLIB_SAFE_MATH=OFF.
//
// Allowed here:
// - reassociation
// - vectorized reductions
// - FMA contraction
//
// Do not put code here if it relies on:
// - NaN/Inf checks
// - signed zero
// - floating-point exceptions
// - exact operation ordering

#define DSPLIB_NO_TYPES_FP_CONSTANTS

#include "dsplib/math.h"

namespace dsplib {

//-------------------------------------------------------------------------------------------------
real_t sum(span_real arr) {
    const size_t n = arr.size();
    real_t sum{0};
    for (size_t i = 0; i < n; ++i) {
        sum += arr[i];
    }
    return sum;
}

cmplx_t sum(span_cmplx arr) {
    const size_t n = arr.size();
    real_t sum_re{0};
    real_t sum_im{0};
    for (size_t i = 0; i < n; ++i) {
        sum_re += arr[i].re;
        sum_im += arr[i].im;
    }
    return {sum_re, sum_im};
}

//-------------------------------------------------------------------------------------------------
real_t dot(span_real x1, span_real x2) {
    DSPLIB_ASSERT(x1.size() == x2.size(), "arrays sizes must be equal");
    const size_t n = x1.size();
    real_t acc = 0;
    for (size_t i = 0; i < n; ++i) {
        acc += x1[i] * x2[i];
    }
    return acc;
}

cmplx_t dot(span_cmplx x1, span_cmplx x2) {
    DSPLIB_ASSERT(x1.size() == x2.size(), "arrays sizes must be equal");
    const size_t n = x1.size();
    real_t acc_re{0};
    real_t acc_im{0};
    for (size_t i = 0; i < n; ++i) {
        const real_t ar = x1[i].re;
        const real_t ai = x1[i].im;
        const real_t br = x2[i].re;
        const real_t bi = x2[i].im;
        acc_re += ar * br - ai * bi;
        acc_im += ar * bi + ai * br;
    }
    return {acc_re, acc_im};
}

//-------------------------------------------------------------------------------------------------
real_t rms(span_real arr) {
    const size_t n = arr.size();
    DSPLIB_ASSUME(n > 1);
    real_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        sum += (arr[i] * arr[i]);
    }
    return std::sqrt(sum / (n - 1));
}

real_t rms(span_cmplx arr) {
    const size_t n = arr.size();
    DSPLIB_ASSUME(n > 1);
    real_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        sum += (arr[i].re * arr[i].re);
        sum += (arr[i].im * arr[i].im);
    }
    return std::sqrt(sum / (n - 1));
}

}   // namespace dsplib