#include "tests_common.h"

//-------------------------------------------------------------------------------------------------
TEST(FFT, FftReal) {
    using namespace dsplib;

    {
        int idx = 10;
        int nfft = 512;
        arr_real x = sin(arange(nfft) * 2 * pi * idx / nfft);
        auto y = fft(x) / nfft;
        auto z = abs(y);
        auto r = zeros(nfft);
        r[idx] = 0.5;
        r[nfft - idx] = 0.5;
        ASSERT_EQ_ARR_REAL(r, z);
    }

    {
        const arr_real x = randn(1024);
        ASSERT_EQ_ARR_CMPLX(fft(x), fft(arr_cmplx(x)));
    }

    {
        const arr_real x = randn(500);
        ASSERT_EQ_ARR_CMPLX(fft(x), fft(arr_cmplx(x)));
    }

    {
        const auto tt = arange(500) * 0.01;
        const arr_real x = sin(2 * pi * tt);
        ASSERT_EQ_ARR_CMPLX(fft(x), fft(arr_cmplx(x)));
    }
}

//-------------------------------------------------------------------------------------------------
TEST(FFT, FftCmplx) {
    using namespace dsplib;
    int idx = 10;
    int nfft = 512;
    auto x = expj(arange(nfft) * 2 * pi * idx / nfft);
    auto y = fft(x) / nfft;
    auto z = abs(y);
    auto r = zeros(nfft);
    r[idx] = 1;
    ASSERT_EQ_ARR_REAL(r, z);
}

//-------------------------------------------------------------------------------------------------
TEST(FFT, Ifft) {
    using namespace dsplib;

    {
        const int nfft = 512;
        auto x = randn(nfft) + 1i * randn(nfft);
        auto y = fft(x);
        auto xx = ifft(y);
        ASSERT_EQ_ARR_CMPLX(x, xx);
    }

    {
        const int nfft = 500;
        arr_cmplx x = randn(nfft) + 1i * randn(nfft);
        auto y = fft(x);
        auto xx = ifft(y);
        ASSERT_EQ_ARR_CMPLX(x, xx);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(FFT, Czt) {
    using namespace dsplib;
    arr_cmplx dft_ref = {6.00000000000000 + 0.00000000000000i, -1.50000000000000 + 0.866025403784439i,
                         -1.50000000000000 - 0.866025403784439i};
    arr_real x = {1.0, 2.0, 3.0};
    const int m = x.size();
    cmplx_t w = expj(-2 * pi / m);
    auto czt_res = czt(x, m, w);
    ASSERT_EQ_ARR_CMPLX(czt_res, dft_ref);
}

//-------------------------------------------------------------------------------------------------
TEST(FFT, CztICzt) {
    using namespace dsplib;
    for (size_t i = 0; i < 1000; i++) {
        int n = randi({16, 2000});
        arr_cmplx x_in = randn(n) + 1i * randn(n);
        auto y = czt(x_in, n, expj(-2 * pi / n));
        auto x_out = czt(y, n, expj(2 * pi / n)) / n;
        ASSERT_EQ_ARR_CMPLX(x_in, x_out);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(FFT, CztFft2) {
    using namespace dsplib;
    for (size_t i = 0; i < 1000; i++) {
        int n = randi({16, 2000});
        n = 1L << nextpow2(n);
        cmplx_t w = expj(-2 * pi / n);
        arr_cmplx x = randn(n) + 1i * randn(n);
        auto y1 = czt(x, n, w);
        auto y2 = fft(x);
        ASSERT_EQ_ARR_CMPLX(y1, y2);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(FFT, CztIFft2) {
    using namespace dsplib;
    for (size_t i = 0; i < 1000; i++) {
        int n = randi({16, 2000});
        n = 1L << nextpow2(n);
        cmplx_t w = expj(2 * pi / n);
        arr_cmplx x = randn(n) + 1i * randn(n);
        auto y1 = czt(x, n, w);
        auto y2 = ifft(x) * n;
        ASSERT_EQ_ARR_CMPLX(y1, y2);
    }
}
