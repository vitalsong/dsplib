#ifndef TESTS_COMMON_H
#define TESTS_COMMON_H

#include <dsplib.h>
#include <gtest/gtest.h>

#define EQ_ABS_ERR (0.000000001)

//-------------------------------------------------------------------------------------------------
template <typename T1, typename T2>
static void ASSERT_EQ_ARR_REAL(const T1& r1, const T2& r2)
{
    ASSERT_EQ(r1.size(), r2.size());
    for (int i=0; i < r1.size(); ++i) {
        EXPECT_NEAR(r1[i], r2[i], EQ_ABS_ERR);
    }
}

//-------------------------------------------------------------------------------------------------
template <typename T1, typename T2>
static void ASSERT_EQ_ARR_CMPLX(const T1& r1, const T2& r2)
{
    ASSERT_EQ(r1.size(), r2.size());
    for (int i=0; i < r1.size(); ++i)
    {
        EXPECT_NEAR(r1[i].re, r2[i].re, EQ_ABS_ERR);
        EXPECT_NEAR(r1[i].im, r2[i].im, EQ_ABS_ERR);
    }
}

#endif // TESTS_COMMON_H
