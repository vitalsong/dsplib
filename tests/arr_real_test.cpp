#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, Init)
{
    arr_real a1;
    arr_real a2{};
    arr_real a3{a1};
    arr_real a4(10);
    arr_real a5 = a4;
    arr_real a6 = a5 + a4;
    arr_real a7{a6 + a4};

    ASSERT_EQ(a1.size(), 0);
    ASSERT_EQ(a2.size(), 0);
    ASSERT_EQ(a3.size(), 0);
    ASSERT_EQ(a4.size(), 10);
    ASSERT_EQ(a5.size(), 10);
    ASSERT_EQ(a6.size(), 10);
    ASSERT_EQ(a7.size(), 10);
    ASSERT_TRUE(a1.empty());
    ASSERT_TRUE(a2.empty());
    ASSERT_TRUE(a3.empty());

    std::vector <short> v1 = {-1, -2, 3, 4};
    arr_real a8{v1.data(), v1.size()};
    ASSERT_EQ_ARR_REAL(v1, a8);

    std::vector <::real_t> v2 = {1, 2, 3, 4};
    arr_real a9{v2};
    ASSERT_EQ_ARR_REAL(v2, a9);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, Arithm)
{
    arr_real a1 = {-1, -2, 3, 4};
    arr_real a2 = {1, 2, -3, -4};

    arr_real sum_r = {0, 0, 0, 0};
    arr_real mlt_r = {-1, -4, -9, -16};
    arr_real div_r = {-1, -1, -1, -1};

    auto sum = a1 + a2;
    auto mlt = a1 * a2;
    auto div = a1 / a2;

    ASSERT_EQ_ARR_REAL(sum, sum_r);
    ASSERT_EQ_ARR_REAL(mlt, mlt_r);
    ASSERT_EQ_ARR_REAL(div, div_r);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, Slice)
{
    arr_real a1 = {0, 1, 2, 3};
    arr_real a1_r = {-100, -100, 2, 3};
    a1.slice(0, 1) = -100;
    ASSERT_EQ_ARR_REAL(a1, a1_r);

    arr_real a2 = {1, 2, 3, 4};
    arr_real a2_r = {1, 2, 100, 200};
    a2.slice(2, 3) = {100, 200};
    ASSERT_EQ_ARR_REAL(a2, a2_r);

    arr_real a3 = {-1, -2, -3, -4};
    arr_real a3_r = {-3, -4, -3, -4};
    a3.slice(0, 1) = a3.slice(2, 3);
    ASSERT_EQ_ARR_REAL(a3, a3_r);

    arr_real a4 = {1, 2, 3, 4};
    arr_real a4_r = {1, 2, 100, 200};
    a4.slice(2, 3) = arr_real{100, 200};
    ASSERT_EQ_ARR_REAL(a4, a4_r);

    arr_real a5 = {0, 1, 2, 3};
    arr_real a5_r = {-1, -2, -3, -4};
    a5 = a5_r.slice(0, 3);
    ASSERT_EQ_ARR_REAL(a5, a5_r);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, Concatenate)
{
    arr_real a1 = {100, 200};
    arr_real a2 = {-100, -200};
    arr_real j1 = {100, 200, -100, -200};
    arr_real j2 = {-100, -200, 100, 200};

    auto r1 = concatenate(a1, a2);
    auto r2 = concatenate(a2, a1);

    ASSERT_EQ_ARR_REAL(r1, j1);
    ASSERT_EQ_ARR_REAL(r2, j2);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, Zeros)
{
    arr_real r = {0, 0, 0, 0};
    auto v = zeros(4);
    ASSERT_EQ_ARR_REAL(r, v);
}
