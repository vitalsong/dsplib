#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(ArrCmplxTest, Init)
{
    ::arr_cmplx a1;
    ::arr_cmplx a2{};
    ::arr_cmplx a3{a1};
    ::arr_cmplx a4(10);
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
        ASSERT_FLOAT_EQ(v1[i], a8[i].re);
        ASSERT_FLOAT_EQ(0, a8[i].im);
    }

    std::vector <::cmplx_t> v2 = {1+1i, 2+2i, 3+3i, 4+4i};
    ::arr_cmplx a9{v2};
    ASSERT_EQ_ARR_CMPLX(v2, a9);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrCmplxTest, ArithmCmplxArr)
{
    arr_cmplx a1 = {-1-1i, -2-2i, 3+3i, 4+4i};
    arr_cmplx a2 = {1+1i, 2+2i, -3-3i, -4-4i};

    arr_cmplx sum_r = {0, 0, 0, 0};
    arr_cmplx mlt_r = {-2i, -8i, -18i, -32i};
    arr_cmplx div_r = {-1, -1, -1, -1};

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
    arr_cmplx a1 = {-1-1i, -2-2i, 3+3i, 4+4i};
    arr_real a2 = {1, 2, -3, -4};

    arr_cmplx sum_r = {-1i, -2i, 3i, 4i};
    arr_cmplx mlt_r = {-1-1i, -4-4i, -9-9i, -16-16i};
    arr_cmplx div_r = {-1-1i, -1-1i, -1-1i, -1-1i};

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
    arr_cmplx a1 = {-1-1i, -2-2i, 3+3i, 4+4i};
    auto a2 = cmplx_t{-1, -1};

    arr_cmplx sum_r = {-2-2i, -3-3i, 2+2i, 3+3i};
    arr_cmplx mlt_r = {2i, 4i, -6i, -8i};
    arr_cmplx div_r = {1, 2, -3, -4};

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
    arr_cmplx a1 = {-1-1i, -2-2i, 3+3i, 4+4i};
    auto a2 = real_t{-2};

    arr_cmplx sum_r = {-3-1i, -4-2i, 1+3i, 2+4i};
    arr_cmplx mlt_r = {2+2i, 4+4i, -6-6i, -8-8i};
    arr_cmplx div_r = {0.5+0.5i, 1+1i, -1.5-1.5i, -2-2i};

    auto sum = a1 + a2;
    auto mlt = a1 * a2;
    auto div = a1 / a2;

    ASSERT_EQ_ARR_CMPLX(sum, sum_r);
    ASSERT_EQ_ARR_CMPLX(mlt, mlt_r);
    ASSERT_EQ_ARR_CMPLX(div, div_r);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrCmplxTest, Concatenate)
{
    arr_cmplx a1 = {100 + 100i, 200 + 200i};
    arr_cmplx a2 = {-100 - 100i, -200 - 200i};
    arr_cmplx j1 = {100 + 100i, 200 + 200i, -100 - 100i, -200 - 200i};
    arr_cmplx j2 = {-100 - 100i, -200 - 200i, 100 + 100i, 200 + 200i};
    
    auto r1 = concatenate(a1, a2);
    auto r2 = concatenate(a2, a1);
    ASSERT_EQ_ARR_CMPLX(r1, j1);
    ASSERT_EQ_ARR_CMPLX(r2, j2);

    ASSERT_EQ_ARR_CMPLX((a1 | a2), j1);
    ASSERT_EQ_ARR_CMPLX((a2 | a1), j2);
    ASSERT_EQ_ARR_CMPLX((zeros(3) | a1), arr_cmplx{0, 0, 0, 100 + 100i, 200 + 200i});
    ASSERT_EQ_ARR_CMPLX((a1 | zeros(3)), arr_cmplx{100 + 100i, 200 + 200i, 0, 0, 0});
}

//-------------------------------------------------------------------------------------------------
TEST(ArrCmplxTest, Zeros)
{
    arr_cmplx r = {0, 0, 0, 0};
    auto v = zeros(4) * 1i;
    ASSERT_EQ_ARR_CMPLX(r, v);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrCmplxTest, UnpackReal)
{
    std::vector<short> r1 = {1, -2, -3, +4};
    arr_real s1(r1.data(), r1.size());
    std::vector<short> r2 = from_real<short>(s1);
    ASSERT_EQ_ARR_REAL(r1, r2);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrCmplxTest, UnpackCmplx)
{
    std::vector<short> s1 = {1, -2, -3, +4};
    arr_cmplx a1 = to_complex(s1);
    arr_cmplx a2 = {1 - 2i, -3 + 4i};
    std::vector<short> s2 = from_complex<short>(a1);
    ASSERT_EQ_ARR_REAL(s1, s2);
    ASSERT_EQ_ARR_CMPLX(a1, a2);
}