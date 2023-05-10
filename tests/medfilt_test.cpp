#include "dsplib/array.h"
#include "dsplib/medfilt.h"
#include "dsplib/utils.h"
#include "tests_common.h"
#include <cmath>
#include <gtest/gtest.h>

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(MedianFilter, OddOrder) {
    {
        arr_real x = {0, 0, 0, -10, 0, 0, 0, 10, 0, 0, -10};
        MedianFilter flt(3);
        auto y = flt(x);
        ASSERT_EQ(max(y), 0);
        ASSERT_EQ(min(y), 0);
        ASSERT_EQ(y.size(), x.size());
    }

    {
        arr_real x = {1, 2, 3, 4, 5};
        MedianFilter flt(3);
        auto y1 = flt(x);
        auto y2 = medfilt(x, 3);
        ASSERT_EQ_ARR_REAL(y1, arr_real{0, 1, 2, 3, 4});
        ASSERT_EQ_ARR_REAL(y2, arr_real{1, 2, 3, 4, 4});
    }

    {
        arr_real x = {1, 1, 1, 1, 1, 1};
        auto y = medfilt(x, 3);
        ASSERT_EQ_ARR_REAL(y, arr_real{1, 1, 1, 1, 1, 1});
    }

    {
        arr_real x = {1, 1, 1, 1, 1, 1};
        auto y = medfilt(x, 11);
        ASSERT_EQ_ARR_REAL(y, arr_real{1, 1, 1, 1, 1, 1});
    }

    {
        arr_real x = {1, 2, 3, 4, 5, 6};
        auto y = medfilt(x, 11);
        ASSERT_EQ_ARR_REAL(y, arr_real{1, 1, 1, 1, 1, 1});
    }

    {
        arr_real x = {1, 1, 1, 1, 1, 1};
        auto y = medfilt(x, 13);
        ASSERT_EQ_ARR_REAL(y, arr_real{0, 0, 0, 0, 0, 0});
    }

    {
        arr_real x = {0, 1, 2, 3, 4, 5};
        auto y = medfilt(x, 3);
        ASSERT_EQ_ARR_REAL(y, arr_real{0, 1, 2, 3, 4, 4});
    }
}

TEST(MedianFilter, EvenOrder) {
    {
        arr_real x = {1, 1, 1, 1, 1, 1};
        auto y = medfilt(x, 4);
        ASSERT_EQ_ARR_REAL(y, arr_real{0.5, 1, 1, 1, 1, 1});
    }
    {
        arr_real x = {1, 2, 3, 4, 5, 6, 7};
        auto y = medfilt(x, 4);
        ASSERT_EQ_ARR_REAL(y, arr_real{0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 5.5});
    }
}

//-------------------------------------------------------------------------------------------------
TEST(MedianFilter, Init) {
    {
        arr_real x = {10, 10};
        MedianFilter flt(7);
        auto y = flt(x);
        ASSERT_EQ(min(y), 0);
        ASSERT_EQ(max(y), 0);
    }

    {
        arr_real x = {10, 10};
        MedianFilter flt(7, 10);
        auto y = flt(x);
        ASSERT_EQ(min(y), 10);
        ASSERT_EQ(max(y), 10);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(MedianFilter, Filter) {
    auto t = arange(1000) / 16000;
    arr_real x = sin(2 * pi * 440 * t);
    x[20] = 10;
    x[30] = -10;

    MedianFilter flt(3);
    auto y = flt(x);

    ASSERT_NEAR(max(x), 10.0, 1e-7);
    ASSERT_NEAR(min(x), -10.0, 1e-7);

    ASSERT_NEAR(max(y), 1.0, 1e-2);
    ASSERT_NEAR(min(y), -1.0, 1e-2);
}
