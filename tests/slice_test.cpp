#include "tests_common.h"
#include <gtest/gtest.h>

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, Slice)
{
    {
        arr_real x1 = {0, 1, 2, 3};
        arr_real x2 = {-100, -100, 2, 3};
        x1.slice(0, 2) = -100;
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
    {
        arr_real x1 = {1, 2, 3, 4};
        arr_real x2 = {1, 2, 100, 200};
        x1.slice(2, 4) = {100, 200};
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
    {
        arr_real x1 = {-1, -2, -3, -4};
        arr_real x2 = {-3, -4, -3, -4};
        x1.slice(0, 2) = x1.slice(2, 4);
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
    {
        arr_real x1 = {1, 2, 3, 4};
        arr_real x2 = {1, 2, 100, 200};
        x1.slice(2, 4) = arr_real{100, 200};
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
    {
        arr_real x1 = {0, 1, 2, 3};
        arr_real x2 = {-1, -2, -3, -4};
        x1 = x2.slice(0, 4);
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(SliceTest, Cmplx)
{
    {
        arr_cmplx x1 = {0, 1 + 1i, 2 + 2i, 3 + 3i};
        arr_cmplx x2 = {-100 - 100i, -100 - 100i, 2 + 2i, 3 + 3i};
        x1.slice(0, 2) = -100 - 100i;
        ASSERT_EQ_ARR_CMPLX(x1, x2);
    }
    {
        arr_cmplx x1 = {1 + 1i, 2 + 2i, 3 + 3i, 4 + 4i};
        arr_cmplx x2 = {1 + 1i, 2 + 2i, 100 + 100i, 200 + 200i};
        x1.slice(2, 4) = {100 + 100i, 200 + 200i};
        ASSERT_EQ_ARR_CMPLX(x1, x2);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, RealNegIndex)
{
    {
        arr_real x = {0, 1, 2, 3};
        ASSERT_FLOAT_EQ(x[-1], 3);
        ASSERT_FLOAT_EQ(x[-2], 2);
        ASSERT_FLOAT_EQ(x[-3], 1);
        ASSERT_FLOAT_EQ(x[-4], 0);
    }
    {
        arr_real x1 = {0, 1, 2, 3};
        arr_real x2 = x1.slice(-2, 2);
        ASSERT_EQ_ARR_REAL(x2, arr_real{2, 3, 0, 1});
        x2 = x1.slice(-1, 1);
        ASSERT_EQ_ARR_REAL(x2, arr_real{3, 0});
        x2 = x1.slice(-1, 0);
        ASSERT_EQ_ARR_REAL(x2, arr_real{3,});
        //TODO?:
        // x2 = x1.slice(2, -1);
        // ASSERT_EQ_ARR_REAL(x2, arr_real{2, 3});
    }
}

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, CmplxNegIndex)
{
    {
        arr_cmplx x = 1i * arr_real{0, 1, 2, 3};
        ASSERT_CMPLX_EQ(x[-1], cmplx_t(3i));
        ASSERT_CMPLX_EQ(x[-2], cmplx_t(2i));
        ASSERT_CMPLX_EQ(x[-3], cmplx_t(1i));
        ASSERT_CMPLX_EQ(x[-4], cmplx_t(0i));
    }
    {
        arr_cmplx x1 = 1i * arr_real{0, 1, 2, 3};
        arr_cmplx x2 = x1.slice(-2, 2);
        ASSERT_EQ_ARR_CMPLX(x2, 1i * arr_real{2, 3, 0, 1});
        x2 = x1.slice(-1, 1);
        ASSERT_EQ_ARR_CMPLX(x2, 1i * arr_real{3, 0});
        x2 = x1.slice(-1, 0);
        ASSERT_EQ_ARR_CMPLX(x2, 1i * arr_real{3,});
    }
}
