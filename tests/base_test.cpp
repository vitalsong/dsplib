#include "tests_common.h"

using namespace dsplib;
using namespace std::complex_literals;

//-------------------------------------------------------------------------------------------------
TEST(BaseTest, CmplxStdLiteral) {
    cmplx_t x1(10i);
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
TEST(BaseTest, CmplxUnary) {
    {
        cmplx_t x1 = 10 + 10i;
        cmplx_t x2 = -x1;
        cmplx_t x3 = conj(x1);
        cmplx_t x4 = +x1;
        ASSERT_CMPLX_EQ(x1, cmplx_t{10, 10});
        ASSERT_CMPLX_EQ(x2, cmplx_t{-10, -10});
        ASSERT_CMPLX_EQ(x3, cmplx_t{10, -10});
        ASSERT_CMPLX_EQ(x4, cmplx_t{10, 10});
        ASSERT_TRUE(x1 == x4);
        ASSERT_TRUE(x1 != x2);
    }
    {
        cmplx_t x1 = 10 + 10i;
        x1 -= 10;
        ASSERT_CMPLX_EQ(x1, cmplx_t{0, 10});
    }
    {
        cmplx_t x1 = 10 + 10i;
        x1 += 10;
        ASSERT_CMPLX_EQ(x1, cmplx_t{20, 10});
    }
}

TEST(BaseTest, CmplxScalarDiv) {
    cmplx_t x1 = 10 + 10i;
    cmplx_t x2 = 1 / x1;
    ASSERT_CMPLX_EQ(x2, cmplx_t{0.05, -0.05});
}

//-------------------------------------------------------------------------------------------------
TEST(BaseTest, LeftScalar) {
    {
        arr_real x = {1, 2, 3, 4};
        ASSERT_EQ_ARR_REAL(2 * x, arr_real{2, 4, 6, 8});
        ASSERT_EQ_ARR_REAL(2 + x, arr_real{3, 4, 5, 6});
        ASSERT_EQ_ARR_REAL(2 - x, arr_real{1, 0, -1, -2});
        ASSERT_EQ_ARR_REAL(2 / x, arr_real{2, 1, 0.666666666666, 0.5});
    }

    {
        cmplx_t d = 2i;
        arr_real x = {2, 4, 5, 8};
        ASSERT_EQ_ARR_CMPLX(d * x, arr_cmplx{4i, 8i, 10i, 16i});
        ASSERT_EQ_ARR_CMPLX(d + x, arr_cmplx{2 + 2i, 4 + 2i, 5 + 2i, 8 + 2i});
        ASSERT_EQ_ARR_CMPLX(d - x, arr_cmplx{-2 + 2i, -4 + 2i, -5 + 2i, -8 + 2i});
        ASSERT_EQ_ARR_CMPLX(d / x, arr_cmplx{1i, 0.5i, 0.4i, 0.25i});
    }
}