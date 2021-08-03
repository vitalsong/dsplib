#include <math.h>
#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Max)
{
    arr_real v1 = {1, 2, 3, 4};
    arr_cmplx v2 = {1+5i, 2, 3, 4};

    ASSERT_EQ(argmax(v1), 3);
    ASSERT_FLOAT_EQ(max(v1), v1[3]);

    ASSERT_EQ(argmax(v2), 0);
    ASSERT_CMPLX_EQ(max(v2), v2[0]);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Min)
{
    arr_real v1 = {1, 2, 3, 4};
    arr_cmplx v2 = {1+5i, 2, 3, 4};

    ASSERT_EQ(argmin(v1), 0);
    ASSERT_FLOAT_EQ(min(v1), v1[0]);

    ASSERT_EQ(argmin(v2), 1);
    ASSERT_CMPLX_EQ(min(v2), v2[1]);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Median)
{
    arr_real v1 = {-1, 0, 1, 2, 3, 4, 5, 6, 7};
    ASSERT_FLOAT_EQ(median(v1), 3);

    arr_real v2 = {0, 0, 0, 1};
    ASSERT_FLOAT_EQ(median(v2), 0);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Std)
{
    arr_real v1 = {5, 5, 5, 5};
    ASSERT_FLOAT_EQ(stddev(v1), 0);

    arr_real v2 = {-1, 1, -1, 1};
    ASSERT_FLOAT_EQ(stddev(v2), 1);

    arr_real v3 = {2, 4, 4, 4, 5, 5, 7, 9};
    ASSERT_FLOAT_EQ(stddev(v3), 2);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Pow2)
{
    arr_real v1 = {5, 5, 5, 5};
    arr_real r1 = {25, 25, 25, 25};
    ASSERT_EQ_ARR_REAL(pow2(v1), r1);

    arr_real v2 = {-1, 1, -1, 1};
    arr_real r2 = {1, 1, 1, 1};
    ASSERT_EQ_ARR_REAL(pow2(v2), r2);

    arr_cmplx v3 = {1+1i, 1, 1i, 0};
    arr_cmplx r3 = {2i, 1, -1, 0};
    ASSERT_EQ_ARR_CMPLX(pow2(v3), r3);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Angle)
{
    arr_cmplx x1 = {1, 1i, 1+1i, -1};
    arr_real y1 = {0, pi/2, pi/4, pi};
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
TEST(MathTest, Exp)
{
    auto x1 = dsplib::exp(1);
    EXPECT_NEAR(x1, 2.7182818284590452, EQ_ABS_ERR);

    auto x2 = dsplib::exp(cmplx_t{0, pi});
    EXPECT_NEAR(x2.re, -1, EQ_ABS_ERR);
    EXPECT_NEAR(x2.im, 0, EQ_ABS_ERR);

    auto x3 = dsplib::expj(pi);
    EXPECT_NEAR(x3.re, -1, EQ_ABS_ERR);
    EXPECT_NEAR(x3.im, 0, EQ_ABS_ERR);

    auto t = range(0, 512) / 8000;
    auto v = t * 2 * pi * 440;
    auto x4 = dsplib::expj(v);
    auto y4 = dsplib::complex(dsplib::cos(v), dsplib::sin(v));
    ASSERT_EQ_ARR_CMPLX(x4, y4);
}