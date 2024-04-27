#include "tests_common.h"

//-------------------------------------------------------------------------------------------------
TEST(FFT, FftReal) {
    using namespace dsplib;
    using namespace std::complex_literals;

    {
        auto x = arr_real{1, 2};
        auto y = fft(x);
        ASSERT_EQ_ARR_CMPLX(y, arr_cmplx{3, -1});
    }

    {
        auto x = arr_real{1, 2, 3, 4, 5};
        auto y1 = fft(x);
        auto y2 = arr_cmplx{15.0000000000000 + 0.00000000000000i, -2.50000000000000 + 3.44095480117793i,
                            -2.50000000000000 + 0.812299240582266i, -2.50000000000000 - 0.812299240582266i,
                            -2.50000000000000 - 3.44095480117793i};
        ASSERT_EQ_ARR_CMPLX(y1, y2);
    }

    {
        int idx = 10;
        int nfft = 512;
        arr_real x = sin(arange(nfft) * 2 * pi * idx / nfft);
        auto y = fft(x) / nfft;
        auto z = abs(y);
        auto r = zeros(nfft);
        r[idx] = 0.5;
        r[nfft - idx] = 0.5;
        ASSERT_EQ_ARR_REAL(r, z, db2mag(-150));
    }

    {
        const arr_real x = {0, 1, 2, 3, 4, 5, 6, 7};
        const arr_cmplx y = {28.0 + 0.0i, -4.0 + 9.65685424949238i, -4.0 + 4.0i, -4.0 + 1.65685424949238i,
                             -4.0 + 0.0i, -4.0 - 1.65685424949238i, -4.0 - 4.0i, -4.0 - 9.65685424949238i};
        ASSERT_EQ_ARR_CMPLX(fft(x), y);
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
        const arr_real x = sin(2 * pi * arange(500) * 0.01);
        ASSERT_EQ_ARR_CMPLX(fft(x), fft(arr_cmplx(x)));
    }
}

//-------------------------------------------------------------------------------------------------
TEST(FFT, FftCmplx) {
    using namespace dsplib;

    {
        int idx = 10;
        int nfft = 512;
        auto x = expj(arange(nfft) * 2 * pi * idx / nfft);
        auto y = fft(x) / nfft;
        auto z = abs(y);
        auto r = zeros(nfft);
        r[idx] = 1;
        ASSERT_EQ_ARR_REAL(r, z);
    }

    {
        const arr_cmplx x = {0 + 0i, 1 + 1i, 2 - 2i, 3 + 3i, 4 - 4i, 5 + 5i, 6 - 6i, 7 + 7i};
        const arr_cmplx y = {
          28.0 + 4.0i,  -5.65685424949238 + 13.6568542494924i, -8.0 + 8.0i, -13.6568542494924 + 5.65685424949238i,
          -4.0 - 28.0i, 5.65685424949238 + 2.34314575050762i,  0.0 + 0.0i,  -2.34314575050762 - 5.65685424949238i};
        ASSERT_EQ_ARR_CMPLX(fft(x), y);
    }

    {
        const int n = 128;
        const arr_cmplx x = sin(2 * pi * arange(n) * 0.1);
        const arr_cmplx y = fft(x).slice(0, n / 2 + 1);
        ASSERT_EQ(argmax(y), 13);
        ASSERT_CMPLX_NEAR(max(y), cmplx_t{-34.4803383753629 - 48.7604203346960i});
    }
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
    arr_cmplx dft_ref = {6.0 + 0.0i, -1.5 + 0.866025403784439i, -1.5 - 0.866025403784439i};
    arr_real x = {1.0, 2.0, 3.0};
    const int m = x.size();
    cmplx_t w = expj(-2 * pi / m);
    auto czt_res = czt(x, m, w);
    ASSERT_EQ_ARR_CMPLX(czt_res, dft_ref);
}

//-------------------------------------------------------------------------------------------------
TEST(FFT, CztPrime) {
    using namespace dsplib;
    using namespace std::complex_literals;

    const int n = 31;
    arr_cmplx x = arange(n);
    arr_cmplx r = {465.000000000000 + 4.97379915032070e-14i, -15.5000000000000 + 152.423942689195i,
                   -15.5000000000000 + 75.4238733733689i,    -15.5000000000000 + 49.4020717291786i,
                   -15.5000000000000 + 36.1192711999180i,    -15.5000000000000 + 27.9256495884725i,
                   -15.5000000000000 + 22.2694576777770i,    -15.5000000000000 + 18.0553388632377i,
                   -15.4999999999999 + 14.7338486721133i,    -15.4999999999999 + 11.9978930641320i,
                   -15.4999999999999 + 9.66122387285297i,    -15.4999999999999 + 7.60311298092457i,
                   -15.4999999999999 + 5.74056963940912i,    -15.4999999999999 + 4.01322805207944i,
                   -15.4999999999999 + 2.37451266120874i,    -15.4999999999997 + 0.786071033534600i,
                   -15.4999999999998 - 0.786071033534697i,   -15.4999999999998 - 2.37451266120878i,
                   -15.4999999999998 - 4.01322805207949i,    -15.4999999999998 - 5.74056963940906i,
                   -15.4999999999998 - 7.60311298092452i,    -15.4999999999998 - 9.66122387285290i,
                   -15.4999999999997 - 11.9978930641320i,    -15.4999999999998 - 14.7338486721132i,
                   -15.4999999999997 - 18.0553388632376i,    -15.4999999999997 - 22.2694576777768i,
                   -15.4999999999997 - 27.9256495884721i,    -15.4999999999997 - 36.1192711999175i,
                   -15.4999999999996 - 49.4020717291777i,    -15.4999999999997 - 75.4238733733674i,
                   -15.5000000000002 - 152.423942689192i};

    auto y = czt(x, n, expj(-2 * pi / n));
    ASSERT_EQ_ARR_CMPLX(y, r);
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
