#include "tests_common.h"
#include <gtest/gtest.h>

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Max) {
    {
        arr_real x = {1, 2, 3, 4};
        ASSERT_EQ(argmax(x), 3);
        ASSERT_FLOAT_EQ(max(x), x[3]);
    }

    {
        arr_cmplx x = {1 + 5i, 2, 3, 4};
        ASSERT_EQ(argmax(x), 0);
        ASSERT_CMPLX_EQ(max(x), x[0]);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Min) {
    {
        arr_real x = {1, 2, 3, 4};
        ASSERT_EQ(argmin(x), 0);
        ASSERT_FLOAT_EQ(min(x), x[0]);
    }

    {
        arr_cmplx x = {1 + 5i, 2, 3, 4};
        ASSERT_EQ(argmin(x), 1);
        ASSERT_CMPLX_EQ(min(x), x[1]);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, PeakToPeak) {
    {
        arr_real x = {-1, -2, 3, 10};
        ASSERT_EQ(peak2peak(x), 12);
    }

    {
        auto t = arange(0, 1, 0.001);
        auto x = cos(2 * pi * 100 * t);
        ASSERT_EQ(peak2peak(x), 2);
    }

    {
        arr_cmplx x = {1 + 1i, 2 + 2i, 3, -4};
        ASSERT_CMPLX_EQ(peak2peak(x), cmplx_t{-5 - 1i});
    }
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Median) {
    {
        arr_real x = {-1, 0, 1, 2, 3, 4, 5, 6, 7};
        ASSERT_FLOAT_EQ(median(x), 3);
    }

    {
        arr_real x = {0, 0, 0, 1};
        ASSERT_FLOAT_EQ(median(x), 0);
    }

    {
        arr_real x = {0, 1, 2, 3};
        ASSERT_FLOAT_EQ(median(x), 1.5);
    }
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
TEST(MathTest, Power2) {
    arr_real v1 = {5, 5, 5, 5};
    arr_real r1 = {25, 25, 25, 25};
    ASSERT_EQ_ARR_REAL(power(v1, 2), r1);

    arr_real v2 = {-1, 1, -1, 1};
    arr_real r2 = {1, 1, 1, 1};
    ASSERT_EQ_ARR_REAL(power(v2, 2), r2);

    arr_cmplx v3 = {1 + 1i, 1, 1i, 0};
    arr_cmplx r3 = {2i, 1, -1, 0};
    ASSERT_EQ_ARR_CMPLX(power(v3, 2), r3);
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

    auto t = arange(0, 512) / 8000;
    auto v = t * 2 * pi * 440;
    auto x4 = dsplib::expj(v);
    auto y4 = dsplib::complex(dsplib::cos(v), dsplib::sin(v));
    ASSERT_EQ_ARR_CMPLX(x4, y4);
}

TEST(MathTest, CmplxExp) {
    arr_cmplx x = {1 + 1i, 2 + 2i, 3 + 3i, 4 + 4i};
    arr_cmplx y = dsplib::exp(x);
    arr_cmplx r = {1.46869393991589 + 2.28735528717884i, -3.07493232063936 + 6.71884969742825i,
                   -19.8845308441470 + 2.83447113248700i, -35.6877324801191 - 41.3200161842802i};
    ASSERT_EQ_ARR_CMPLX(y, r);
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
    auto x = expj(2 * pi * 440 * arange(10000) / 8000.0);
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

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Pow2db) {
    {
        arr_real db = {-10, 0, 10, 20};
        arr_real mag = {0.3162, 1.0000, 3.1623, 10.0000};
        arr_real pow = {0.1000, 1.0000, 10.0000, 100.0000};

        ASSERT_EQ_ARR_REAL(db2pow(db), pow, 1e-3);
        ASSERT_EQ_ARR_REAL(db2mag(db), mag, 1e-3);
        ASSERT_EQ_ARR_REAL(mag2db(mag), db, 1e-3);
        ASSERT_EQ_ARR_REAL(pow2db(pow), db, 1e-3);

        ASSERT_NEAR(db2pow(db(0)), pow(0), 1e-3);
        ASSERT_NEAR(db2mag(db(0)), mag(0), 1e-3);
        ASSERT_NEAR(mag2db(mag(0)), db(0), 1e-3);
        ASSERT_NEAR(pow2db(pow(0)), db(0), 1e-3);
    }
}

TEST(MathTest, Eps) {
    ///double precision
    ASSERT_NEAR(eps(0.5), std::pow(2.0, -53), 1e-15);
    ASSERT_NEAR(eps(1.0), std::pow(2.0, -52), 1e-15);
    ASSERT_NEAR(eps(-1.0), std::pow(2.0, -52), 1e-15);
    ASSERT_NEAR(eps(2.0), std::pow(2.0, -51), 1e-15);
    ASSERT_NEAR(eps(-2.0), std::pow(2.0, -51), 1e-15);
    ASSERT_NEAR(eps(1000.0), std::pow(2.0, -43), 1e-15);
    ASSERT_NEAR(eps(-1000.0), std::pow(2.0, -43), 1e-15);
    ASSERT_NEAR(eps(0.0), std::pow(2.0, -1074), 1e-15);

    //single precision
    ASSERT_NEAR(eps(0.5f), std::pow(2.0, -24), 1e-15);
    ASSERT_NEAR(eps(1.0f), std::pow(2.0, -23), 1e-15);
    ASSERT_NEAR(eps(2.0f), std::pow(2.0, -22), 1e-15);
    ASSERT_NEAR(eps(1000.0f), std::pow(2.0, -14), 1e-15);
    ASSERT_NEAR(eps(0.0f), std::pow(2.0, -149), 1e-15);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, CumSum) {
    {
        const arr_real x1 = {1, 2, 3, 4, 5};
        const arr_real x2 = {1, 3, 6, 10, 15};
        ASSERT_EQ_ARR_REAL(cumsum(x1), x2);
    }
    {
        const arr_real x1 = {1, 2, 3, 4, 5};
        const arr_real x2 = {15, 14, 12, 9, 5};
        ASSERT_EQ_ARR_REAL(cumsum(x1, Direction::Reverse), x2);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Corr) {
    {
        const arr_real x = randn(100);
        ASSERT_EQ(corr(x, x), 1.0);
        ASSERT_EQ(corr(x, -x), -1.0);
    }
    {
        const arr_real x = {0, 1, 5, 6, 9, 11, 3, 2};
        ASSERT_EQ(corr(x, x, Correlation::Spearman), 1.0);
        ASSERT_EQ(corr(x, -x, Correlation::Spearman), -1.0);
    }
    {
        const arr_real x1 = {1, 2, 3, 4, 5, 6};
        const arr_real x2 = {3, 1, 4, 2, 6, 5};
        ASSERT_NEAR(corr(x1, x2, Correlation::Kendall), 0.466, 1e-3);
        ASSERT_NEAR(corr(x1, -x2, Correlation::Kendall), -0.466, 1e-3);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Sort) {
    {
        const arr_real x1 = {4, 1, 2, 5, 6, 7, 3, 3};
        auto [sorted, index] = sort(x1);
        ASSERT_EQ_ARR_REAL(sorted, arr_real{1, 2, 3, 3, 4, 5, 6, 7});
        ASSERT_EQ_ARR_REAL(index, arr_real{1, 2, 6, 7, 0, 3, 4, 5});
        ASSERT_TRUE(issorted(sorted));
    }
    {
        const arr_real x1 = {4, 1, 2, 5, 6, 7, 3, 3};
        auto [sorted, index] = sort(x1, Direction::Descend);
        ASSERT_EQ_ARR_REAL(sorted, arr_real{7, 6, 5, 4, 3, 3, 2, 1});
        ASSERT_EQ_ARR_REAL(index, arr_real{5, 4, 3, 0, 6, 7, 2, 1});
        ASSERT_TRUE(issorted(sorted, Direction::Descend));
        ASSERT_FALSE(issorted(sorted, Direction::Ascend));
    }
    {
        //sorted
        const arr_real x1 = {0, 1, 2, 3, 4, 5};
        auto [sorted, index] = sort(x1);
        ASSERT_EQ_ARR_REAL(sorted, arr_real{0, 1, 2, 3, 4, 5});
        ASSERT_EQ_ARR_REAL(index, arr_real{0, 1, 2, 3, 4, 5});
        ASSERT_TRUE(issorted(sorted));
        ASSERT_FALSE(issorted(sorted, Direction::Descend));
    }
    {
        //sorted
        const arr_real x1 = {5, 4, 3, 2, 1, 0};
        auto [sorted, index] = sort(x1, Direction::Descend);
        ASSERT_EQ_ARR_REAL(sorted, arr_real{5, 4, 3, 2, 1, 0});
        ASSERT_EQ_ARR_REAL(index, arr_real{0, 1, 2, 3, 4, 5});
        ASSERT_TRUE(issorted(sorted, Direction::Descend));
        ASSERT_FALSE(issorted(sorted, Direction::Ascend));
    }
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Factor) {
    ASSERT_EQ_ARR_REAL(factor(0), arr_int{0});
    ASSERT_EQ_ARR_REAL(factor(1), arr_int{1});
    ASSERT_EQ_ARR_REAL(factor(2), arr_int{2});
    ASSERT_EQ_ARR_REAL(factor(3), arr_int{3});
    ASSERT_EQ_ARR_REAL(factor(11), arr_int{11});
    ASSERT_EQ_ARR_REAL(factor(121), arr_int{11, 11});
    ASSERT_EQ_ARR_REAL(factor(120), arr_int{2, 2, 2, 3, 5});
    ASSERT_EQ_ARR_REAL(factor(128), arr_int{2, 2, 2, 2, 2, 2, 2});
    ASSERT_EQ_ARR_REAL(factor(143), arr_int{11, 13});
    ASSERT_EQ_ARR_REAL(factor(65535), arr_int{3, 5, 17, 257});
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, IsPrime) {
    ASSERT_TRUE(isprime(2));
    ASSERT_TRUE(isprime(3));
    ASSERT_TRUE(isprime(5));
    ASSERT_TRUE(isprime(7));
    ASSERT_TRUE(isprime(11));
    ASSERT_TRUE(isprime(257));
    ASSERT_TRUE(isprime(997));
    ASSERT_TRUE(isprime(1048583));

    ASSERT_FALSE(isprime(0));
    ASSERT_FALSE(isprime(1));
    ASSERT_FALSE(isprime(4));
    ASSERT_FALSE(isprime(12));
    ASSERT_FALSE(isprime(1024));
}

TEST(MathTest, NextPrime) {
    ASSERT_EQ(nextprime(256), 257);
    ASSERT_EQ(nextprime(257), 257);
    ASSERT_EQ(nextprime(1000), 1009);
    ASSERT_EQ(nextprime(1009), 1009);
    ASSERT_EQ(nextprime(1048576), 1048583);
}

TEST(MathTest, Primes) {
    ASSERT_EQ_ARR_INT(primes(4), arr_int{2, 3});
    ASSERT_EQ_ARR_INT(primes(5), arr_int{2, 3, 5});
    ASSERT_EQ_ARR_INT(primes(6), arr_int{2, 3, 5});
    ASSERT_EQ_ARR_INT(primes(7), arr_int{2, 3, 5, 7});
    ASSERT_EQ(primes(1L << 20).size(), 82025);
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, AnyInf) {
    {
        arr_real x = {0, 1, 2, 3};
        ASSERT_FALSE(anyinf(x));
    }
    {
        arr_real x = {0, 1, 2, inf};
        ASSERT_TRUE(anyinf(x));
    }
    {
        arr_real x = {0, 1, 2, -inf};
        ASSERT_TRUE(anyinf(x));
    }
    {
        arr_cmplx x = {0i, 1i, 2i, inf};
        ASSERT_TRUE(anyinf(x));
    }
}

TEST(MathTest, AnyNan) {
    {
        arr_real x = {0, 1, 2, NAN};
        ASSERT_TRUE(anynan(x));
    }
    {
        arr_cmplx x = {0i, 1i, 2i, NAN};
        ASSERT_TRUE(anynan(x));
    }
}

//-------------------------------------------------------------------------------------------------
TEST(MathTest, Log) {
    arr_real x = {1, 2, 3, 4, 5};
    arr_real y = dsplib::log(x);
    arr_real r = {0, 0.693147180559945, 1.09861228866811, 1.38629436111989, 1.60943791243410};
    ASSERT_EQ_ARR_REAL(y, r);
}

TEST(MathTest, Log2) {
    arr_real x = {1, 2, 3, 4, 5};
    arr_real y = dsplib::log2(x);
    arr_real r = {0, 1, 1.58496250072116, 2, 2.32192809488736};
    ASSERT_EQ_ARR_REAL(y, r);
}

TEST(MathTest, Sqrt) {
    arr_real x = {0, 1, 2, 3, 4, 5};
    arr_real y = dsplib::sqrt(abs2(x));
    ASSERT_EQ_ARR_REAL(y, x);
}