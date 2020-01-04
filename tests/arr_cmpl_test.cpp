#ifndef ARR_CMPL_TEST_H
#define ARR_CMPL_TEST_H

#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(ArrCmplxTest, Init)
{
    ::arr_cmplx a1;
    ::arr_cmplx a2{};
    ::arr_cmplx a3{a1};
    ::arr_cmplx a4{10};
    ::arr_cmplx a5 = a4;
    ::arr_cmplx a6 = a5 + a4;
    ::arr_cmplx a7{a6 + a4};

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
    ::arr_cmplx a8{v1.data(), v1.size()};
    for (int i=0; i < 4; ++i)
    {
        ASSERT_FLOAT_EQ(v1[i], a8[i].xi);
        ASSERT_FLOAT_EQ(0, a8[i].xq);
    }

    std::vector <::cmplx_t> v2 = {{1, 1}, {2, 2}, {3, 3}, {4, 4}};
    ::arr_cmplx a9{v2};
    ASSERT_EQ_ARR_CMPLX(v2, a9);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrCmplxTest, ArithmCmplxArr)
{
    auto a1 = ::arr_cmplx::init({{-1, -1}, {-2, -2}, {3, 3}, {4, 4}});
    auto a2 = ::arr_cmplx::init({{1, 1}, {2, 2}, {-3, -3}, {-4, -4}});

    auto sum_r = ::arr_cmplx::init({{0, 0}, {0, 0}, {0, 0}, {0, 0}});
    auto mlt_r = ::arr_cmplx::init({{0, -2}, {0, -8}, {0, -18}, {0, -32}});
    auto div_r = ::arr_cmplx::init({{-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}});

    auto sum = a1 + a2;
    auto mlt = a1 * a2;
    auto div = a1 / a2;

    ASSERT_EQ_ARR_CMPLX(sum, sum_r);
    ASSERT_EQ_ARR_CMPLX(mlt, mlt_r);
    ASSERT_EQ_ARR_CMPLX(div, div_r);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrCmplxTest, ArithmRealArr)
{
    auto a1 = ::arr_cmplx::init({{-1, -1}, {-2, -2}, {3, 3}, {4, 4}});
    auto a2 = ::arr_real::init({1, 2, -3, -4});

    auto sum_r = ::arr_cmplx::init({{0, -1}, {0, -2}, {0, 3}, {0, 4}});
    auto mlt_r = ::arr_cmplx::init({{-1, -1}, {-4, -4}, {-9, -9}, {-16, -16}});
    auto div_r = ::arr_cmplx::init({{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}});

    auto sum = a1 + a2;
    auto mlt = a1 * a2;
    auto div = a1 / a2;
    ASSERT_EQ_ARR_CMPLX(sum, sum_r);
    ASSERT_EQ_ARR_CMPLX(mlt, mlt_r);
    ASSERT_EQ_ARR_CMPLX(div, div_r);

    sum = a2 + a1;
    mlt = a2 * a1;
    ASSERT_EQ_ARR_CMPLX(sum, sum_r);
    ASSERT_EQ_ARR_CMPLX(mlt, mlt_r);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrCmplxTest, ArithmCmplxValue)
{
    auto a1 = ::arr_cmplx::init({{-1, -1}, {-2, -2}, {3, 3}, {4, 4}});
    auto a2 = cmplx_t{-1, -1};

    auto sum_r = ::arr_cmplx::init({{-2, -2}, {-3, -3}, {2, 2}, {3, 3}});
    auto mlt_r = ::arr_cmplx::init({{0, 2}, {0, 4}, {0, -6}, {0, -8}});
    auto div_r = ::arr_cmplx::init({{1, 0}, {2, 0}, {-3, 0}, {-4, 0}});

    auto sum = a1 + a2;
    auto mlt = a1 * a2;
    auto div = a1 / a2;

    ASSERT_EQ_ARR_CMPLX(sum, sum_r);
    ASSERT_EQ_ARR_CMPLX(mlt, mlt_r);
    ASSERT_EQ_ARR_CMPLX(div, div_r);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrCmplxTest, ArithmRealValue)
{
    auto a1 = ::arr_cmplx::init({{-1, -1}, {-2, -2}, {3, 3}, {4, 4}});
    auto a2 = real_t{-2};

    auto sum_r = ::arr_cmplx::init({{-3, -1}, {-4, -2}, {1, 3}, {2, 4}});
    auto mlt_r = ::arr_cmplx::init({{2, 2}, {4, 4}, {-6, -6}, {-8, -8}});
    auto div_r = ::arr_cmplx::init({{0.5, 0.5}, {1, 1}, {-1.5, -1.5}, {-2, -2}});

    auto sum = a1 + a2;
    auto mlt = a1 * a2;
    auto div = a1 / a2;

    ASSERT_EQ_ARR_CMPLX(sum, sum_r);
    ASSERT_EQ_ARR_CMPLX(mlt, mlt_r);
    ASSERT_EQ_ARR_CMPLX(div, div_r);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrCmplxTest, Set)
{
    auto a1 = ::arr_cmplx::init({{0, 0}, {1, 1}, {2, 2}, {3, 3}});
    auto a1_r = ::arr_cmplx::init({{-100, -100}, {-100, -100}, {2, 2}, {3, 3}});
    a1.set(0, 1, {-100, -100});
    ASSERT_EQ_ARR_CMPLX(a1, a1_r);

    auto a2 = ::arr_cmplx::init({{1, 1}, {2, 2}, {3, 3}, {4, 4}});
    auto a2_r = ::arr_cmplx::init({{1, 1}, {2, 2}, {100, 100}, {200, 200}});
    a2.set(2, 3, ::arr_cmplx::init({{100, 100}, {200, 200}}));
    ASSERT_EQ_ARR_CMPLX(a2, a2_r);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrCmplxTest, Join)
{
    auto a1 = ::arr_cmplx::init({{100, 100}, {200, 200}});
    auto a2 = ::arr_cmplx::init({{-100, -100}, {-200, -200}});
    auto j1 = ::arr_cmplx::init({{100, 100}, {200, 200}, {-100, -100}, {-200, -200}});
    auto j2 = ::arr_cmplx::init({{-100, -100}, {-200, -200}, {100, 100}, {200, 200}});

    auto r1 = ::arr_cmplx::join(a1, a2);
    auto r2 = ::arr_cmplx::join(a2, a1);

    ASSERT_EQ_ARR_CMPLX(r1, j1);
    ASSERT_EQ_ARR_CMPLX(r2, j2);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrCmplxTest, Zeros)
{
    auto r = ::arr_cmplx::init({{0, 0}, {0, 0}, {0, 0}, {0, 0}});
    auto v = ::arr_cmplx::zeros(4);
    ASSERT_EQ_ARR_CMPLX(r, v);
}

#endif // ARR_CMPL_TEST_H
