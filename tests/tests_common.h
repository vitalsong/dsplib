#ifndef TESTS_COMMON_H
#define TESTS_COMMON_H

#include <dsplib.h>
#include <gtest/gtest.h>

//-------------------------------------------------------------------------------------------------
template <typename T1, typename T2>
static void ASSERT_EQ_ARR_REAL(const T1& r1, const T2& r2)
{
    ASSERT_EQ(r1.size(), r2.size());
    for (int i=0; i < r1.size(); ++i) {
        ASSERT_FLOAT_EQ(r1[i], r2[i]);
    }
}

//-------------------------------------------------------------------------------------------------
template <typename T1, typename T2>
static void ASSERT_EQ_ARR_CMPLX(const T1& r1, const T2& r2)
{
    ASSERT_EQ(r1.size(), r2.size());
    for (int i=0; i < r1.size(); ++i)
    {
        ASSERT_FLOAT_EQ(r1[i].xi, r2[i].xi);
        ASSERT_FLOAT_EQ(r1[i].xq, r2[i].xq);
    }
}

#endif // TESTS_COMMON_H
