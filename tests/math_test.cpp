#ifndef MATH_TEST_H
#define MATH_TEST_H

#include <math.h>
#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Max)
{
    auto v1 = ::arr_real::init({1, 2, 3, 4});
    auto v2 = ::arr_cmplx::init({1+5i, 2, 3, 4});

    ASSERT_EQ(::argmax(v1), 3);
    ASSERT_FLOAT_EQ(::max(v1), v1[3]);

    ASSERT_EQ(::argmax(v2), 0);
    ASSERT_CMPLX_EQ(::max(v2), v2[0]);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Min)
{
    auto v1 = ::arr_real::init({1, 2, 3, 4});
    auto v2 = ::arr_cmplx::init({1+5i, 2, 3, 4});

    ASSERT_EQ(::argmin(v1), 0);
    ASSERT_FLOAT_EQ(::min(v1), v1[0]);

    ASSERT_EQ(::argmin(v2), 1);
    ASSERT_CMPLX_EQ(::min(v2), v2[1]);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Median)
{
    auto v1 = ::arr_real::init({-1, 0, 1, 2, 3, 4, 5, 6, 7});
    ASSERT_FLOAT_EQ(::median(v1), 3);

    auto v2 = ::arr_real::init({0, 0, 0, 1});
    ASSERT_FLOAT_EQ(::median(v2), 0);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Std)
{
    auto v1 = ::arr_real::init({5, 5, 5, 5});
    ASSERT_FLOAT_EQ(::stddev(v1), 0);

    auto v2 = ::arr_real::init({-1, 1, -1, 1});
    ASSERT_FLOAT_EQ(::stddev(v2), 1);

    auto v3 = ::arr_real::init({2, 4, 4, 4, 5, 5, 7, 9});
    ASSERT_FLOAT_EQ(::stddev(v3), 2);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Pow2)
{
    auto v1 = ::arr_real::init({5, 5, 5, 5});
    auto r1 = ::arr_real::init({25, 25, 25, 25});
    ASSERT_EQ_ARR_REAL(::pow2(v1), r1);

    auto v2 = ::arr_real::init({-1, 1, -1, 1});
    auto r2 = ::arr_real::init({1, 1, 1, 1});
    ASSERT_EQ_ARR_REAL(::pow2(v2), r2);

    auto v3 = ::arr_cmplx::init({1+1i, 1, 1i, 0});
    auto r3 = ::arr_cmplx::init({2i, 1, -1, 0});
    ASSERT_EQ_ARR_CMPLX(::pow2(v3), r3);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Angle)
{
    auto x1 = ::arr_cmplx::init({1, 1i, 1+1i, -1});
    auto y1 = ::arr_real::init({0, M_PI/2, M_PI/4, M_PI});
    ASSERT_EQ_ARR_REAL(abs(angle(x1)), y1);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Randn)
{
    auto x = randn(32767);
    auto s = stddev(x);
    EXPECT_NEAR(s, 1, 0.02f);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Range)
{
    auto x1 = range(0, 10);
    auto xx1 = range(10);
    auto y1 = ::arr_real::init({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    ASSERT_EQ_ARR_REAL(x1, y1);
    ASSERT_EQ_ARR_REAL(xx1, y1);

    auto x2 = range(-1, 0, 0.1);
    auto y2 = ::arr_real::init({-1, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1});
    ASSERT_EQ_ARR_REAL(x2, y2);

    auto x3 = range(-1, 1, 0.5);
    auto y3 = ::arr_real::init({-1, -0.5, 0, 0.5});
    ASSERT_EQ_ARR_REAL(x3, y3);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Exp)
{
    auto x1 = dsplib::exp(1);
    EXPECT_NEAR(x1, 2.7182818284590452, EQ_ABS_ERR);

    auto x2 = dsplib::exp(cmplx_t{0, M_PI});
    EXPECT_NEAR(x2.re, -1, EQ_ABS_ERR);
    EXPECT_NEAR(x2.im, 0, EQ_ABS_ERR);

    auto x3 = dsplib::expj(M_PI);
    EXPECT_NEAR(x3.re, -1, EQ_ABS_ERR);
    EXPECT_NEAR(x3.im, 0, EQ_ABS_ERR);

    auto t = range(0, 512) / 8000;
    auto v = t * 2 * M_PI * 440;
    auto x4 = dsplib::expj(v);
    auto y4 = dsplib::complex(dsplib::cos(v), dsplib::sin(v));
    ASSERT_EQ_ARR_CMPLX(x4, y4);
}

#endif // MATH_TEST_H
