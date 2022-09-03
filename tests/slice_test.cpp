#include "tests_common.h"
#include <gtest/gtest.h>

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(SliceTest, Base) {
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
TEST(SliceTest, Cmplx) {
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
TEST(SliceTest, NegIndex) {
    {
        arr_real x = {0, 1, 2, 3};
        ASSERT_FLOAT_EQ(x[-1], 3);
        ASSERT_FLOAT_EQ(x[-2], 2);
        ASSERT_FLOAT_EQ(x[-3], 1);
        ASSERT_FLOAT_EQ(x[-4], 0);
    }

    {
        arr_real x = {0, 1, 2, 3};
        arr_real r = x.slice(0, -1);
        ASSERT_EQ_ARR_REAL(r, arr_real{0, 1, 2});
    }

    {
        arr_real x = {0, 1, 2, 3};
        arr_real r = x.slice(0, -2);
        ASSERT_EQ_ARR_REAL(r, arr_real{0, 1});
    }

    {
        arr_real x = {0, 1, 2, 3, 4};
        arr_real r = x.slice(-3, -1);
        ASSERT_EQ_ARR_REAL(r, arr_real{2, 3});
    }
}

//-------------------------------------------------------------------------------------------------
TEST(SliceTest, NegStep) {
    {
        arr_real x = range(6);
        arr_real r = x.slice(4, 0, -1);
        ASSERT_EQ_ARR_REAL(r, arr_real{4, 3, 2, 1});
    }
    {
        arr_real x = range(6);
        arr_real r = x.slice(4, 0, -2);
        ASSERT_EQ_ARR_REAL(r, arr_real{4, 2});
    }
    {
        arr_real x = range(5);
        arr_real r = x.slice(4, 0, -2);
        ASSERT_EQ_ARR_REAL(r, arr_real{4, 2});
    }
    {
        arr_real x = range(7);
        arr_real r = x.slice(-6, 0, -1);
        ASSERT_EQ_ARR_REAL(r, arr_real{
                                1,
                              });
    }
    {
        arr_real x = range(7);
        arr_real r = x.slice(-7, 7);
        ASSERT_EQ_ARR_REAL(r, arr_real{0, 1, 2, 3, 4, 5, 6});
    }
}

//-------------------------------------------------------------------------------------------------
TEST(SliceTest, CmplxNegIndex) {
    {
        arr_cmplx x = 1i * arr_real{0, 1, 2, 3};
        ASSERT_CMPLX_EQ(x[-1], cmplx_t(3i));
        ASSERT_CMPLX_EQ(x[-2], cmplx_t(2i));
        ASSERT_CMPLX_EQ(x[-3], cmplx_t(1i));
        ASSERT_CMPLX_EQ(x[-4], cmplx_t(0i));
    }
}

//-------------------------------------------------------------------------------------------------
TEST(SliceTest, ConstSlice) {
    const arr_real x1 = {0, 1, 2, 3};
    arr_real x2 = x1.slice(1, 3);
    ASSERT_EQ_ARR_REAL(x2, arr_real{1, 2});
}

//-------------------------------------------------------------------------------------------------
TEST(SliceTest, Overlaped) {
    {
        arr_real x = {0, 1, 2, 3};
        x.slice(0, 2) = x.slice(2, 4);
        ASSERT_EQ_ARR_REAL(x, arr_real{2, 3, 2, 3});
    }

    {
        arr_real x = {3, 2, 1, 0};
        x.slice(2, 4) = x.slice(0, 2);
        ASSERT_EQ_ARR_REAL(x, arr_real{3, 2, 3, 2});
    }

    {
        arr_real x = {0, 1, 2, 3, 4, 5};
        x.slice(0, 4) = x.slice(2, 6);
        ASSERT_EQ_ARR_REAL(x, arr_real{2, 3, 4, 5, 4, 5});
    }

    {
        arr_real x = {0, 1, 2, 3, 4, 5};
        x.slice(2, 6) = x.slice(0, 4);
        ASSERT_EQ_ARR_REAL(x, arr_real{0, 1, 0, 1, 2, 3});
    }

    {
        arr_real x = {0, 1, 2, 3, 4, 5, 6, 7};
        x.slice(2, 8, 2) = x.slice(0, 6, 2);
        ASSERT_EQ_ARR_REAL(x, arr_real{0, 1, 0, 3, 2, 5, 4, 7});
    }

    {
        arr_real x = {0, 1, 2, 3, 4, 5, 6, 7};
        x.slice(0, 6, 2) = x.slice(1, 7, 2);
        ASSERT_EQ_ARR_REAL(x, arr_real{1, 1, 3, 3, 5, 5, 6, 7});
    }
}

//-------------------------------------------------------------------------------------------------
TEST(SliceTest, Placeholders) {
    using namespace indexing;
    const arr_real x1 = {0, 1, 2, 3};
    arr_real x2 = x1.slice(1, end);
    ASSERT_EQ_ARR_REAL(x2, arr_real{1, 2, 3});
}