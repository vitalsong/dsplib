#ifndef ARR_REAL_TEST_H
#define ARR_REAL_TEST_H

#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, Init)
{
    ::arr_real a1;
    ::arr_real a2{};
    ::arr_real a3{a1};
    ::arr_real a4{10};
    ::arr_real a5 = a4;
    ::arr_real a6 = a5 + a4;
    ::arr_real a7{a6 + a4};

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
    ::arr_real a8{v1.data(), v1.size()};
    ASSERT_EQ_ARR_REAL(v1, a8);

    std::vector <::real_t> v2 = {1, 2, 3, 4};
    ::arr_real a9{v2};
    ASSERT_EQ_ARR_REAL(v2, a9);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, Arithm)
{
    auto a1 = ::arr_real::init({-1, -2, 3, 4});
    auto a2 = ::arr_real::init({1, 2, -3, -4});

    auto sum_r = ::arr_real::init({0, 0, 0, 0});
    auto mlt_r = ::arr_real::init({-1, -4, -9, -16});
    auto div_r = ::arr_real::init({-1, -1, -1, -1});

    auto sum = a1 + a2;
    auto mlt = a1 * a2;
    auto div = a1 / a2;

    ASSERT_EQ_ARR_REAL(sum, sum_r);
    ASSERT_EQ_ARR_REAL(mlt, mlt_r);
    ASSERT_EQ_ARR_REAL(div, div_r);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, Set)
{
    auto a1 = ::arr_real::init({0, 1, 2, 3});
    auto a1_r = ::arr_real::init({-100, -100, 2, 3});
    a1.set(0, 1, -100);
    ASSERT_EQ_ARR_REAL(a1, a1_r);

    auto a2 = ::arr_real::init({1, 2, 3, 4});
    auto a2_r = ::arr_real::init({1, 2, 100, 200});
    a2.set(2, 3, ::arr_real::init({100, 200}));
    ASSERT_EQ_ARR_REAL(a2, a2_r);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, Join)
{
    auto a1 = ::arr_real::init({100, 200});
    auto a2 = ::arr_real::init({-100, -200});
    auto j1 = ::arr_real::init({100, 200, -100, -200});
    auto j2 = ::arr_real::init({-100, -200, 100, 200});

    auto r1 = ::arr_real::join(a1, a2);
    auto r2 = ::arr_real::join(a2, a1);

    ASSERT_EQ_ARR_REAL(r1, j1);
    ASSERT_EQ_ARR_REAL(r2, j2);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, Zeros)
{
    auto r = ::arr_real::init({0, 0, 0, 0});
    auto v = ::arr_real::zeros(4);
    ASSERT_EQ_ARR_REAL(r, v);
}

#endif // ARR_REAL_TEST_H
