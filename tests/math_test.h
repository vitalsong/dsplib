#ifndef MATH_TEST_H
#define MATH_TEST_H

#include <gtest/gtest.h>
#include <dsplib.h>
#include <math.h>
#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Max)
{
    auto v1 = ::arr_real::init({1, 2, 3, 4});
    auto v2 = ::arr_cmplx::init({{1, 5}, {2, 0}, {3, 0}, {4, 0}});

    ASSERT_EQ(::argmax(v1), 3);
    ASSERT_FLOAT_EQ(::max(v1), v1[3]);

    ASSERT_EQ(::argmax(v2), 0);
    ASSERT_FLOAT_EQ(::max(v2).xi, v2[0].xi);
    ASSERT_FLOAT_EQ(::max(v2).xq, v2[0].xq);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Min)
{
    auto v1 = ::arr_real::init({1, 2, 3, 4});
    auto v2 = ::arr_cmplx::init({{1, 5}, {2, 0}, {3, 0}, {4, 0}});

    ASSERT_EQ(::argmin(v1), 0);
    ASSERT_FLOAT_EQ(::min(v1), v1[0]);

    ASSERT_EQ(::argmin(v2), 1);
    ASSERT_FLOAT_EQ(::min(v2).xi, v2[1].xi);
    ASSERT_FLOAT_EQ(::min(v2).xq, v2[1].xq);
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

    auto v3 = ::arr_cmplx::init({{1, 1}, {1, 0}, {0, 1}, {0, 0}});
    auto r3 = ::arr_cmplx::init({{0, 2}, {1, 0}, {-1, 0}, {0, 0}});
    ASSERT_EQ_ARR_CMPLX(::pow2(v3), r3);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Angle)
{
    auto x1 = ::arr_cmplx::init({{1, 0}, {0, 1}, {1, 1}, {-1, 0}});
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

#endif // MATH_TEST_H
