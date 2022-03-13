#include "tests_common.h"

#include <dsplib.h>

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(XCorrTest, Real) {
    {
        arr_real r = {0, 1, 4, 7, 6};
        arr_real x1 = {0, 1, 2};
        arr_real x2 = {3, 2, 1};
        arr_real y = xcorr(x1, x2);
        ASSERT_EQ_ARR_REAL(y, r);
    }

    {
        arr_real r = {8, 23, 44, 70, 56, 39, 20};
        arr_real x1 = {1, 2, 3, 4};
        arr_real x2 = {5, 6, 7, 8};
        arr_real y = xcorr(x1, x2);
        ASSERT_EQ_ARR_REAL(y, r);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(XCorrTest, Cmplx) {
    arr_cmplx x1 = {0 + 1i, 1 - 2i, 2 + 6i, 3 - 5i, 4 - 1i};
    arr_cmplx x2 = {5 - 3i, 6 + 1i, 7 - 3i, 8 - 1i, 9 + 2i};
    arr_cmplx r = {2 + 9i, 4 - 12i, 37 + 42i, 41 - 6i, 60 - 14i, 98 - 3i, 36 + 8i, 53 - 26i, 23 + 7i};
    arr_cmplx y = xcorr(x1, x2);
    ASSERT_EQ_ARR_CMPLX(y, r);
}

//-------------------------------------------------------------------------------------------------
TEST(XCorrTest, Auto) {
    arr_cmplx x = {0 + 1i, 1 - 2i, 2 + 6i, 3 - 5i, 4 - 1i};
    arr_cmplx r = {-1 + 4i, 1 - 4i, 21 + 27i, -19 + 2i, 97 + 0i, -19 - 2i, 21 - 27i, 1 + 4i, -1 - 4i};
    arr_cmplx y = xcorr(x);
    ASSERT_EQ_ARR_CMPLX(y, r);
}