#include "dsplib/array.h"
#include "dsplib/utils.h"
#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(Utils, Range) {
    {
        auto x1 = range(0, 10);
        auto x2 = range(10);
        arr_real x3 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        ASSERT_EQ_ARR_REAL(x1, x3);
        ASSERT_EQ_ARR_REAL(x2, x3);
    }
    {
        auto x1 = range(-1, 0, 0.1);
        arr_real x2 = {-1, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1};
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
    {
        auto x1 = range(-1, 1, 0.5);
        arr_real x2 = {-1, -0.5, 0, 0.5};
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(Utils, Zeros) {
    auto x1 = zeros(5);
    arr_real x2 = {0, 0, 0, 0, 0};
    ASSERT_EQ_ARR_REAL(x1, x2);
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
    auto freq = 440.0;
    auto fs = 8e3;
    auto t = range(1024) / fs;
    auto x = sin(2 * pi * freq * t);
    x = awgn(x, 20);
    auto X = fft(x);
    auto n = argmax(X);
    auto freq_q = peakloc(X, n) / X.size() * fs;
    auto freq_r = peakloc(abs(X), n) / X.size() * fs;
    ASSERT_NEAR(freq, freq_q, 0.1);
    ASSERT_NEAR(freq, freq_r, 2);
}