#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Max) {
    arr_real v1 = {1, 2, 3, 4};
    arr_cmplx v2 = {1 + 5i, 2, 3, 4};

    ASSERT_EQ(argmax(v1), 3);
    ASSERT_FLOAT_EQ(max(v1), v1[3]);

    ASSERT_EQ(argmax(v2), 0);
    ASSERT_CMPLX_EQ(max(v2), v2[0]);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Min) {
    arr_real v1 = {1, 2, 3, 4};
    arr_cmplx v2 = {1 + 5i, 2, 3, 4};

    ASSERT_EQ(argmin(v1), 0);
    ASSERT_FLOAT_EQ(min(v1), v1[0]);

    ASSERT_EQ(argmin(v2), 1);
    ASSERT_CMPLX_EQ(min(v2), v2[1]);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Median) {
    arr_real v1 = {-1, 0, 1, 2, 3, 4, 5, 6, 7};
    ASSERT_FLOAT_EQ(median(v1), 3);

    arr_real v2 = {0, 0, 0, 1};
    ASSERT_FLOAT_EQ(median(v2), 0);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Std) {
    {
        arr_real v1 = {5, 5, 5, 5};
        ASSERT_FLOAT_EQ(stddev(v1), 0);

        arr_real v2 = {-1, 1, -1, 1};
        ASSERT_FLOAT_EQ(stddev(v2), 1.154700538379252);

        arr_real v3 = {2, 4, 4, 4, 5, 5, 7, 9};
        ASSERT_FLOAT_EQ(stddev(v3), 2.138089935299395);
    }

    {
        arr_cmplx v1 = {5i, 5, 5i, 5};
        ASSERT_FLOAT_EQ(stddev(v1), 4.082482904638630);

        arr_cmplx v2 = {-1i, 1, -1, 1i};
        ASSERT_FLOAT_EQ(stddev(v2), 1.154700538379252);

        arr_cmplx v3 = {2, 1 + 4i, 4, 2 + 4i, 2 + 5i, 3 + 5i, 7, 9};
        ASSERT_FLOAT_EQ(stddev(v3), 3.722518348798681);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Pow2) {
    arr_real v1 = {5, 5, 5, 5};
    arr_real r1 = {25, 25, 25, 25};
    ASSERT_EQ_ARR_REAL(pow2(v1), r1);

    arr_real v2 = {-1, 1, -1, 1};
    arr_real r2 = {1, 1, 1, 1};
    ASSERT_EQ_ARR_REAL(pow2(v2), r2);

    arr_cmplx v3 = {1 + 1i, 1, 1i, 0};
    arr_cmplx r3 = {2i, 1, -1, 0};
    ASSERT_EQ_ARR_CMPLX(pow2(v3), r3);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Angle) {
    arr_cmplx x1 = {1, 1i, 1 + 1i, -1};
    arr_real y1 = {0, pi / 2, pi / 4, pi};
    ASSERT_EQ_ARR_REAL(abs(angle(x1)), y1);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, NormalDist) {
    auto x = randn(1e6);
    auto s = stddev(x);
    EXPECT_NEAR(s, 1, 1e-2);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, UniformDist) {
    auto x = rand(1e6);
    auto s = stddev(x);
    EXPECT_NEAR(s, 0.2887, 1e-2);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, UniformIntDist) {
    int imax = 1e4;
    auto x = randi(imax, 1e6);
    auto xf = arr_real(x);
    auto s = stddev(xf);
    EXPECT_GE(min(xf), 1);
    EXPECT_LE(max(xf), imax);
    EXPECT_NEAR(s / imax, 0.2887, 1e-2);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Exp) {
    auto x1 = dsplib::exp(1);
    EXPECT_NEAR(x1, 2.7182818284590452, EQ_ABS_ERR);

    auto x2 = dsplib::exp(cmplx_t{0, pi});
    EXPECT_NEAR(x2.re, -1, EQ_ABS_ERR);
    EXPECT_NEAR(x2.im, 0, EQ_ABS_ERR);

    auto x3 = dsplib::expj(pi);
    EXPECT_NEAR(x3.re, -1, EQ_ABS_ERR);
    EXPECT_NEAR(x3.im, 0, EQ_ABS_ERR);

    auto t = range(0, 512) / 8000;
    auto v = t * 2 * pi * 440;
    auto x4 = dsplib::expj(v);
    auto y4 = dsplib::complex(dsplib::cos(v), dsplib::sin(v));
    ASSERT_EQ_ARR_CMPLX(x4, y4);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Downsample) {
    {
        auto r = downsample(arr_real{1, 2, 3, 4}, 1);
        ASSERT_EQ_ARR_REAL(arr_real{1, 2, 3, 4}, r);
    }

    {
        auto r = downsample(arr_real{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, 3);
        ASSERT_EQ_ARR_REAL(arr_real{1, 4, 7, 10}, r);
    }

    {
        auto r = downsample(arr_cmplx{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, 3, 2);
        ASSERT_EQ_ARR_CMPLX(arr_cmplx{3, 6, 9}, r);
    }

    {
        auto r = downsample(arr_real{0, 1, 2, 3, 4, 5, 6}, 3, 1);
        ASSERT_EQ_ARR_REAL(arr_real{1, 4}, r);
    }

    {
        auto r = downsample(arr_real{0, 1, 2, 3, 4, 5, 6, 7}, 3, 1);
        ASSERT_EQ_ARR_REAL(arr_real{1, 4, 7}, r);
    }

    {
        auto r = downsample(arr_real{0, 1, 2, 3, 4, 5, 6, 8}, 3, 0);
        ASSERT_EQ_ARR_REAL(arr_real{0, 3, 6}, r);
    }

    {
        auto r = downsample(arr_real{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, 3, 0);
        ASSERT_EQ_ARR_REAL(arr_real{0, 3, 6, 9}, r);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Upsample) {
    {
        auto r = upsample(arr_real{0, 1, 2, 3}, 2);
        ASSERT_EQ_ARR_REAL(arr_real{0, 0, 1, 0, 2, 0, 3, 0}, r);
    }

    {
        auto r = upsample(arr_real{0, 1, 2, 3}, 2, 1);
        ASSERT_EQ_ARR_REAL(arr_real{0, 0, 0, 1, 0, 2, 0, 3}, r);
    }

    {
        auto r = upsample(arr_real{0, 1, 2, 3}, 1, 0);
        ASSERT_EQ_ARR_REAL(arr_real{0, 1, 2, 3}, r);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Rms) {
    auto x = expj(2 * pi * 440 * range(10000) / 8000.0);
    auto y = real(x);
    auto rms_x = rms(x);
    auto rms_y = rms(y);
    ASSERT_NEAR(rms_x, 1, 0.0001);
    ASSERT_NEAR(rms_y, 0.707142132106764, 0.0001);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Abs2) {
    arr_cmplx x = {10 + 10i, 1 - 2i, 3 + 4i};
    arr_real r = {200, 5, 25};
    ASSERT_EQ_ARR_REAL(abs2(x), r);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Deg2Rad) {
    arr_real deg = {0, 45, 90, 180, -45, -90, -180};
    arr_real rad = {0, pi / 4, pi / 2, pi, -pi / 4, -pi / 2, -pi};
    ASSERT_EQ_ARR_REAL(deg2rad(deg), rad);
    ASSERT_EQ_ARR_REAL(rad2deg(rad), deg);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Norm) {
    {
        arr_real v = {1, -2, 3};
        real_t n = norm(v);
        ASSERT_NEAR(n, 3.7417, 0.0001);
    }

    {
        arr_cmplx v = {1 + 2i, -2 - 4i, 3 - 6i};
        real_t n = norm(v);
        ASSERT_NEAR(n, 8.3666, 0.0001);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, MSE) {
    {
        arr_real x = {1, -2, 3};
        arr_real y = {1, -2, 3};
        ASSERT_NEAR(mse(x, y), 0, 0.0001);
    }

    {
        arr_real x = {1, -2, 3};
        arr_real y = {-1, 2, -3};
        ASSERT_NEAR(mse(x, y), 18.6666, 0.0001);
        ASSERT_NEAR(nmse(x, y), 1.3333, 0.0001);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Dot) {
    arr_real x1 = {-1, 2, -3, 4};
    arr_real x2 = {2, 3, 4, 5};
    ASSERT_EQ(dot(x1, x2), sum(x1 * x2));
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Sign) {
    ASSERT_EQ(sign(-1.0), -1);
    ASSERT_EQ(sign(2.0), 1);
    ASSERT_EQ(sign(0), 0);

    {
        auto x = cmplx_t{1, 1};
        auto r = cmplx_t{0.7071, 0.7071};
        ASSERT_NEAR(real(sign(x)), real(r), 1e-4);
        ASSERT_NEAR(imag(sign(x)), imag(r), 1e-4);
    }

    {
        auto x = cmplx_t{0, 0};
        auto r = cmplx_t{0, 0};
        ASSERT_NEAR(real(sign(x)), real(r), 1e-4);
        ASSERT_NEAR(imag(sign(x)), imag(r), 1e-4);
    }

    {
        auto x = cmplx_t{2, -3};
        auto r = cmplx_t{0.5547, -0.8321};
        ASSERT_NEAR(real(sign(x)), real(r), 1e-4);
        ASSERT_NEAR(imag(sign(x)), imag(r), 1e-4);
    }
}