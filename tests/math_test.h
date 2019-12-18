#ifndef MATH_TEST_H
#define MATH_TEST_H

#include <gtest/gtest.h>
#include <dsplib.h>
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

#endif // MATH_TEST_H
