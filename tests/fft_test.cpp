#include "tests_common.h"
#include <math.h>

//-------------------------------------------------------------------------------------------------
TEST(MathTest, FftReal)
{
    using namespace dsplib;
    int idx = 10;
    int nfft = 512;
    auto x = sin(range(nfft) * 2 * pi * idx / nfft);
    auto y = fft(x) / nfft;
    auto z = abs(y);
    auto r = zeros(nfft);
    r[idx] = 0.5;
    r[nfft-idx] = 0.5;
    ASSERT_EQ_ARR_REAL(r, z);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, FftCmplx)
{
    using namespace dsplib;
    int idx = 10;
    int nfft = 512;
    auto x = expj(range(nfft) * 2 * pi * idx / nfft);
    auto y = fft(x) / nfft;
    auto z = abs(y);
    auto r = zeros(nfft);
    r[idx] = 1;
    ASSERT_EQ_ARR_REAL(r, z);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Ifft)
{
    using namespace dsplib;
    int nfft = 512;
    auto x = complex(randn(nfft), randn(nfft));
    auto y = fft(x);
    auto xx = ifft(y);
    ASSERT_EQ_ARR_CMPLX(x, xx);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Czt)
{
    using namespace dsplib;
    arr_cmplx dft_ref = {6.00000000000000 + 0.00000000000000i, -1.50000000000000 + 0.866025403784439i, -1.50000000000000 - 0.866025403784439i};
    arr_real x = {1.0, 2.0, 3.0};
    const int m = x.size();
    cmplx_t w = expj(-2 * pi / m);
    auto czt_res = czt(x, m, w);
    ASSERT_EQ_ARR_CMPLX(czt_res, dft_ref);
}
