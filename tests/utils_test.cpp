#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(Utils, Range) {
    {
        auto x1 = arange(0, 10);
        auto x2 = arange(10);
        arr_real x3 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        ASSERT_EQ_ARR_REAL(x1, x3);
        ASSERT_EQ_ARR_REAL(x2, x3);
    }
    {
        auto x1 = arange(-1, 0, 0.1);
        arr_real x2 = {-1, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1};
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
    {
        auto x1 = arange(-1, 1, 0.5);
        arr_real x2 = {-1, -0.5, 0, 0.5};
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
    {
        auto x1 = arange(0, 100, 2);
        auto x2 = arange(1, 100, 2);
        ASSERT_EQ(x1.size(), 50);
        ASSERT_EQ(x2.size(), 50);
    }
    {
        auto x1 = arange(0, 100, 2.0);
        auto x2 = arange(1, 100, 2.0);
        ASSERT_EQ(x1.size(), 50);
        ASSERT_EQ(x2.size(), 50);
    }
    {
        auto x1 = arange(0, 16000.0, 16000.0 / 200000);
        ASSERT_EQ(x1.size(), 200000);
        auto last_tgt = 16000.0 - 16000.0 / 200000;
        ASSERT_NEAR(x1[-1], last_tgt, 1e-3);
    }
    {
        ASSERT_EQ_ARR_REAL(arange(-10, 0, 2), arr_real{-10, -8, -6, -4, -2});
        ASSERT_EQ_ARR_REAL(arange(-10, -1, 2), arr_real{-10, -8, -6, -4, -2});
        ASSERT_EQ_ARR_REAL(arange(-10, 1, 2), arr_real{-10, -8, -6, -4, -2, 0});
    }
    {
        ASSERT_EQ_ARR_REAL(arange(0, -10, -2), arr_real{0, -2, -4, -6, -8});
        ASSERT_EQ_ARR_REAL(arange(-1, -10, -2), arr_real{-1, -3, -5, -7, -9});
        ASSERT_EQ_ARR_REAL(arange(-2, -10, -2), arr_real{-2, -4, -6, -8});
        ASSERT_EQ_ARR_REAL(arange(-2, -11, -2), arr_real{-2, -4, -6, -8, -10});
    }
}

//-------------------------------------------------------------------------------------------------
TEST(Utils, Zeros) {
    auto x1 = zeros(5);
    arr_real x2 = {0, 0, 0, 0, 0};
    ASSERT_EQ_ARR_REAL(x1, x2);
}

//-------------------------------------------------------------------------------------------------
TEST(Utils, Zeropad) {
    {
        arr_real x1 = {1, 2, 3, 4, 5};
        arr_real x2 = zeropad(x1, 5);
        arr_real x3 = zeropad(x1, 7);
        ASSERT_EQ_ARR_REAL(x2, arr_real{1, 2, 3, 4, 5});
        ASSERT_EQ_ARR_REAL(x3, arr_real{1, 2, 3, 4, 5, 0, 0});
    }
    {
        arr_cmplx x1 = {1i, 2i, 3i, 4i, 5i};
        arr_cmplx x2 = zeropad(x1, 5);
        arr_cmplx x3 = zeropad(x1, 7);
        ASSERT_EQ_ARR_CMPLX(x2, arr_cmplx{1i, 2i, 3i, 4i, 5i});
        ASSERT_EQ_ARR_CMPLX(x3, arr_cmplx{1i, 2i, 3i, 4i, 5i, 0, 0});
    }
}

//-------------------------------------------------------------------------------------------------
TEST(Utils, RepelemReal) {
    {
        auto x1 = repelem(arr_real{1.0, 2, 3, 4}, 1);
        arr_real x2 = {1, 2, 3, 4};
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
    {
        auto x1 = repelem(arr_real{1.0, 2, 3, 4}, 0);
        arr_real x2 = {};
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
    {
        auto x1 = repelem(arr_real{1.0, 2, 3, 4}, 2);
        arr_real x2 = {1, 1, 2, 2, 3, 3, 4, 4};
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(Utils, RepelemCmplx) {
    {
        auto x1 = repelem(arr_real{1.0, 2, 3, 4} * 1i, 1);
        arr_cmplx x2 = 1i * arr_real{1, 2, 3, 4};
        ASSERT_EQ_ARR_CMPLX(x1, x2);
    }
    {
        auto x1 = repelem(arr_real{1.0, 2, 3, 4} * 1i, 0);
        arr_cmplx x2 = {};
        ASSERT_EQ_ARR_CMPLX(x1, x2);
    }
    {
        auto x1 = repelem(arr_real{1.0, 2, 3, 4} * 1i, 2);
        arr_cmplx x2 = 1i * arr_real{1, 1, 2, 2, 3, 3, 4, 4};
        ASSERT_EQ_ARR_CMPLX(x1, x2);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(Utils, Flip) {
    {
        arr_real x1 = {1, 2, 3, 4};
        auto x2 = flip(arr_real{4, 3, 2, 1});
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
    {
        arr_cmplx x1 = {1, 2, 3, 4};
        auto x2 = flip(arr_cmplx{4, 3, 2, 1});
        ASSERT_EQ_ARR_CMPLX(x1, x2);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(Utils, FromFile) {
    {
        int16_t s[] = {1, -1, -100, 100, 200, 300};

        auto fid = fopen("test.dat", "wb");
        fwrite(s, sizeof(int16_t), 6, fid);
        fclose(fid);

        auto x1 = arr_real(s, 6);
        arr_real x2 = from_file("test.dat", dtype::int16, endian::little);
        ASSERT_EQ_ARR_REAL(x1, x2);
    }

    {
        uint16_t s[] = {1, 2, 3, 4, 5, 6};

        auto fid = fopen("test.dat", "wb");
        fwrite(s, sizeof(uint16_t), 6, fid);
        fclose(fid);

        auto x1 = arr_real(s + 1, 5);
        arr_real x2 = from_file("test.dat", dtype::uint16, endian::little, sizeof(uint16_t));
        ASSERT_EQ_ARR_REAL(x1, x2);
    }

    {
        int32_t s[] = {0, INT32_MAX, INT32_MIN, 1, 0, 0};

        auto fid = fopen("test.dat", "wb");
        fwrite(s, sizeof(int32_t), 6, fid);
        fclose(fid);

        auto x1 = arr_real(s + 1, 3);
        arr_real x2 = from_file("test.dat", dtype::int32, endian::little, sizeof(int32_t), 3);
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(Utils, Peakloc) {
    const int N = 2048;
    auto freq = 440.0;
    auto fs = 8e3;
    auto t = arange(N) / fs;
    auto x = sin(2 * pi * freq * t);
    x = awgn(x, 20);
    auto X = fft(x);
    X.slice(N / 2, N) = 0;
    auto n = argmax(X);
    auto freq_q = peakloc(X, n) / X.size() * fs;
    auto freq_r = peakloc(abs(X), n) / X.size() * fs;
    ASSERT_NEAR(freq, freq_q, 0.1);
    ASSERT_NEAR(freq, freq_r, 2);
}

//-------------------------------------------------------------------------------------------------
TEST(Utils, Finddelay) {
    {
        auto x1 = dsplib::randn(1000);
        auto x2 = dsplib::awgn(x1, 20);
        auto d = dsplib::finddelay(x1, x2);
        ASSERT_EQ(d, 0);
    }

    {
        auto x1 = dsplib::randn(1000);
        auto x2 = dsplib::awgn(x1, 20);
        x2 = dsplib::delayseq(x2, 5);
        auto d = dsplib::finddelay(x1, x2);
        ASSERT_EQ(d, 5);
    }

    {
        auto x1 = dsplib::randn(1000);
        auto x2 = dsplib::awgn(x1, 20);
        x2 = dsplib::delayseq(x2, -10);
        auto d = dsplib::finddelay(x1, x2);
        ASSERT_EQ(d, -10);
    }

    {
        dsplib::arr_real x = {1, 2, 3};
        dsplib::arr_real y = {0, 0, 1, 2, 3};
        auto d = dsplib::finddelay(x, y);
        ASSERT_EQ(d, 2);
    }

    {
        arr_real x = {0, 0, 0, 1, 2, 3, 0, 0};
        arr_real y = {1, 2, 3, 0};
        auto d = dsplib::finddelay(x, y);
        ASSERT_EQ(d, -3);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(Utils, Linspace) {
    {
        const arr_real x = linspace(-5, 5, 10);
        ASSERT_EQ_ARR_REAL(
          x, arr_real{-5.0000, -3.8889, -2.7778, -1.6667, -0.5556, 0.5556, 1.6667, 2.7778, 3.8889, 5.0000}, 1e-3);
    }
    {
        const arr_real x = linspace(0, 10, 10);
        ASSERT_EQ_ARR_REAL(x, arr_real{0, 1.1111, 2.2222, 3.3333, 4.4444, 5.5556, 6.6667, 7.7778, 8.8889, 10.0000},
                           1e-3);
    }
    {
        const arr_real x = linspace(-5, 5, 1);
        ASSERT_EQ_ARR_REAL(x, arr_real{5}, 1e-3);
    }
    {
        const arr_real x = linspace(-5, 5, 2);
        ASSERT_EQ_ARR_REAL(x, arr_real{-5, 5}, 1e-3);
    }
}