#include "dsplib/array.h"
#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(UtilsTest, Range)
{
    {
        auto x1 = range(0, 10);
        auto x2 = range(10);
        arr_real x3 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        ASSERT_EQ_ARR_REAL(x1, x3);
        ASSERT_EQ_ARR_REAL(x2, x3);
    }
    {
        auto x1 = range(-1, 0, 0.1);
        arr_real x2 = {-1, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1};
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
    {
        auto x1 = range(-1, 1, 0.5);
        arr_real x2 = {-1, -0.5, 0, 0.5};
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(UtilsTest, Zeros)
{
    auto x1 = zeros(5);
    arr_real x2 = {0, 0, 0, 0, 0};
    ASSERT_EQ_ARR_REAL(x1, x2);
}

//-------------------------------------------------------------------------------------------------
TEST(UtilsTest, RepelemReal)
{
    {
        auto x1 = repelem(arr_real{1.0, 2, 3, 4}, 1);
        arr_real x2 = {1, 2, 3, 4};
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
    {
        auto x1 = repelem(arr_real{1.0, 2, 3, 4}, 0);
        arr_real x2 = {};
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
    {
        auto x1 = repelem(arr_real{1.0, 2, 3, 4}, 2);
        arr_real x2 = {1, 1, 2, 2, 3, 3, 4, 4};
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(UtilsTest, RepelemCmplx)
{
    {
        auto x1 = repelem(arr_real{1.0, 2, 3, 4} * 1i, 1);
        arr_cmplx x2 = 1i * arr_real{1, 2, 3, 4};
        ASSERT_EQ_ARR_CMPLX(x1, x2);
    }
    {
        auto x1 = repelem(arr_real{1.0, 2, 3, 4} * 1i, 0);
        arr_cmplx x2 = {};
        ASSERT_EQ_ARR_CMPLX(x1, x2);
    }
    {
        auto x1 = repelem(arr_real{1.0, 2, 3, 4} * 1i, 2);
        arr_cmplx x2 = 1i * arr_real{1, 1, 2, 2, 3, 3, 4, 4};
        ASSERT_EQ_ARR_CMPLX(x1, x2);
    }
}