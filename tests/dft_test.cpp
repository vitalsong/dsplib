#include "tests_common.h"
#include <math.h>

//-------------------------------------------------------------------------------------------------
TEST(DftTest, DftReal)
{
    using namespace dsplib;
    int idx = 10;
    int ndft = 200;
    auto x = sin(range(ndft) * 2 * M_PI * idx / ndft);
    auto y = dft(x) / ndft;
    auto z = abs(y);
    auto r = zeros(ndft);
    r[idx] = 0.5;
    r[ndft-idx] = 0.5;
    ASSERT_EQ_ARR_REAL(r, z);
}

//-------------------------------------------------------------------------------------------------
TEST(DftTest, DftCmplx)
{
    using namespace dsplib;
    int idx = 10;
    int ndft = 311;
    auto x = expj(range(ndft) * 2 * M_PI * idx / ndft);
    auto y = dft(x) / ndft;
    auto z = abs(y);
    auto r = zeros(ndft);
    r[idx] = 1;
    ASSERT_EQ_ARR_REAL(r, z);
}

//-------------------------------------------------------------------------------------------------
TEST(DftTest, Idft)
{
    using namespace dsplib;
    int ndft = 200;
    auto x = complex(randn(ndft), randn(ndft));
    auto y = dft(x);
    auto xx = idft(y);
    ASSERT_EQ_ARR_CMPLX(x, xx);
}

