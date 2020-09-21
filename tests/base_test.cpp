#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(BaseTest, CmplxStdLiteral)
{
    dsplib::cmplx_t x1(10i);
    ASSERT_CMPLX_EQ(x1, cmplx_t{0, 10});

    cmplx_t x2 = 2 - 1i;
    ASSERT_CMPLX_EQ(x2, cmplx_t{2, -1});

    cmplx_t x3{-5i - 5};
    ASSERT_CMPLX_EQ(x3, cmplx_t{-5, -5});

    dsplib::cmplx_t x4 = std::complex<real_t>(10 + 10.0i);
    ASSERT_CMPLX_EQ(x4, cmplx_t{10, 10});

    std::complex<real_t> x5 = cmplx_t{10, -10};
    ASSERT_EQ(x5.real(), 10);
    ASSERT_EQ(x5.imag(), -10);
}

//-------------------------------------------------------------------------------------------------
TEST(BaseTest, PushBack)
{
    dsplib::arr_cmplx x1 = {1i, 2i, 3i, 4i};
    dsplib::arr_real x2 = {1, 2, 3, 4};

    x1.push_back(5i);
    x2.push_back(5);

    ASSERT_EQ_ARR_CMPLX(x1, arr_cmplx{1i, 2i, 3i, 4i, 5i});
    ASSERT_EQ_ARR_REAL(x2, arr_real{1, 2, 3, 4, 5});
}

//-------------------------------------------------------------------------------------------------
TEST(BaseTest, PushFront)
{
    dsplib::arr_cmplx x1 = {1i, 2i, 3i, 4i};
    dsplib::arr_real x2 = {1, 2, 3, 4};

    x1.push_front(0i);
    x2.push_front(0);

    ASSERT_EQ_ARR_CMPLX(x1, arr_cmplx{0i, 1i, 2i, 3i, 4i});
    ASSERT_EQ_ARR_REAL(x2, arr_real{0, 1, 2, 3, 4});
}

//-------------------------------------------------------------------------------------------------
TEST(BaseTest, PopBack)
{
    dsplib::arr_cmplx x1 = {1i, 2i, 3i, 4i};
    dsplib::arr_real x2 = {1, 2, 3, 4};

    auto v1 = x1.pop_back();
    auto v2 = x2.pop_back();

    ASSERT_CMPLX_EQ(v1, cmplx_t{4i});
    ASSERT_EQ(v2, 4);

    ASSERT_EQ_ARR_CMPLX(x1, arr_cmplx{1i, 2i, 3i});
    ASSERT_EQ_ARR_REAL(x2, arr_real{1, 2, 3});
}

//-------------------------------------------------------------------------------------------------
TEST(BaseTest, PopFront)
{
    dsplib::arr_cmplx x1 = {1i, 2i, 3i, 4i};
    dsplib::arr_real x2 = {1, 2, 3, 4};

    auto v1 = x1.pop_front();
    auto v2 = x2.pop_front();

    ASSERT_CMPLX_EQ(v1, cmplx_t{1i});
    ASSERT_EQ(v2, 1);

    ASSERT_EQ_ARR_CMPLX(x1, arr_cmplx{2i, 3i, 4i});
    ASSERT_EQ_ARR_REAL(x2, arr_real{2, 3, 4});
}