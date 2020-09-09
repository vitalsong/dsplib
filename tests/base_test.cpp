#include "tests_common.h"

//-------------------------------------------------------------------------------------------------
TEST(BaseTest, CmplxStdLiteral)
{
    using namespace dsplib;

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
