#ifndef FFT_TEST_H
#define FFT_TEST_H

#include "tests_common.h"
#include <math.h>

//-------------------------------------------------------------------------------------------------
TEST(BaseTest, CmplxUserLiteral)
{
    using namespace dsplib;

    cmplx_t x1(10_j);
    ASSERT_CMPLX_EQ(x1, cmplx_t{0, 10});

    cmplx_t x2 = -1_j + 2;
    ASSERT_CMPLX_EQ(x2, cmplx_t{2, -1});

    cmplx_t x3{-5_j - 5};
    ASSERT_CMPLX_EQ(x3, cmplx_t{-5, -5});
}

//-------------------------------------------------------------------------------------------------
#if __cplusplus > 201309
TEST(BaseTest, CmplxStdLiteral)
{
    using namespace dsplib;

    dsplib::cmplx_t x1(10j);
    ASSERT_CMPLX_EQ(x1, cmplx_t{0, 10});

    cmplx_t x2 = 2 - 1j;
    ASSERT_CMPLX_EQ(x2, cmplx_t{2, -1});

    cmplx_t x3{-5j - 5};
    ASSERT_CMPLX_EQ(x3, cmplx_t{-5, -5});

    dsplib::cmplx_t x4 = std::complex<real_t>(10 + 10.0j);
    ASSERT_CMPLX_EQ(x4, cmplx_t{10, 10});

    std::complex<real_t> x5 = cmplx_t{10, -10};
    ASSERT_EQ(x5.real(), 10);
    ASSERT_EQ(x5.imag(), -10);
}
#endif

#endif // FFT_TEST_H
