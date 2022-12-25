#pragma once

#include <dsplib.h>
#include <gtest/gtest.h>

#define EQ_ABS_ERR (1e-7)

using namespace std::complex_literals;

//-------------------------------------------------------------------------------------------------
template<typename T1, typename T2>
static void ASSERT_CMPLX_EQ(const T1& val1, const T2& val2) {
    ASSERT_EQ(val1.re, val2.re);
    ASSERT_EQ(val1.im, val2.im);
}

//-------------------------------------------------------------------------------------------------
template<typename T1, typename T2>
static void ASSERT_EQ_ARR_REAL(const T1& r1, const T2& r2, double max_err = EQ_ABS_ERR) {
    dsplib::arr_real x1(r1);
    dsplib::arr_real x2(r2);
    ASSERT_EQ(x1.size(), x2.size());
    for (int i = 0; i < x1.size(); ++i) {
        ASSERT_NEAR(x1[i], x2[i], max_err);
    }
}

//-------------------------------------------------------------------------------------------------
template<typename T1, typename T2>
static void ASSERT_EQ_ARR_CMPLX(const T1& r1, const T2& r2, double max_err = EQ_ABS_ERR) {
    dsplib::arr_cmplx x1(r1);
    dsplib::arr_cmplx x2(r2);
    ASSERT_EQ(x1.size(), x2.size());
    for (int i = 0; i < x1.size(); ++i) {
        ASSERT_NEAR(x1[i].re, x2[i].re, max_err);
        ASSERT_NEAR(x1[i].im, x2[i].im, max_err);
    }
}
