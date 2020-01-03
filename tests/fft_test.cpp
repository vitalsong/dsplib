#ifndef FFT_TEST_H
#define FFT_TEST_H

#include "tests_common.h"
#include <math.h>

//-------------------------------------------------------------------------------------------------
TEST(MathTest, FftReal)
{
    using namespace dsplib;
    int idx = 10;
    int nfft = 512;
    auto x = sin(range(nfft) * 2 * M_PI * idx / nfft);
    auto y = fft(x) / nfft;
    auto z = abs(y);
    auto r = arr_real::zeros(nfft);
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
    auto re = cos(range(nfft) * 2 * M_PI * idx / nfft);
    auto im = sin(range(nfft) * 2 * M_PI * idx / nfft);
    auto x = complex(re, im);
    auto y = fft(x) / nfft;
    auto z = abs(y);
    auto r = arr_real::zeros(nfft);
    r[idx] = 1;
    ASSERT_EQ_ARR_REAL(r, z);
}

#endif // FFT_TEST_H
