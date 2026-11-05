#include "tests_common.h"

//-------------------------------------------------------------------------------------------------
TEST(Awgn, Sinad) {
    {
        const int fs = 8000;
        auto tt = dsplib::arange(fs) / fs;
        auto x = 0.1 * dsplib::sin(2 * dsplib::pi * 1200 * tt);
        x = dsplib::awgn(x, 10);
        auto sinad = dsplib::sinad(x);
        ASSERT_NEAR(sinad, 10, 0.5);
    }

    {
        const int fs = 8000;
        auto tt = dsplib::arange(fs) / fs;
        auto x = 0.1 * dsplib::sin(2 * dsplib::pi * 1200 * tt);
        x = dsplib::awgn(x, 25);
        auto sinad = dsplib::sinad(x);
        ASSERT_NEAR(sinad, 25, 0.5);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(Awgn, ComplexSnr) {
    using namespace dsplib;
    const arr_cmplx x = ones(100000) * cmplx_t(2, -3);
    for (real_t snr : {-5, 0, 25}) {
        SCOPED_TRACE(::testing::Message() << "snr=" << snr);
        rng(42);
        const auto y = awgn(x, snr);
        const auto noise = y - x;
        ASSERT_NEAR(mag2db(rms(x) / rms(noise)), snr, 0.1);

        const auto expected_stddev = std::sqrt(13.0 / 2) * std::pow(10, -snr / 20);
        ASSERT_NEAR(rms(real(noise)) / expected_stddev, 1, 0.02);
        ASSERT_NEAR(rms(imag(noise)) / expected_stddev, 1, 0.02);
        ASSERT_NEAR(mean(real(noise)) / expected_stddev, 0, 0.02);
        ASSERT_NEAR(mean(imag(noise)) / expected_stddev, 0, 0.02);

        auto inplace_y = x;
        rng(42);
        awgn(inplace(inplace_y), snr);
        ASSERT_EQ_ARR_CMPLX(inplace_y, y);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(Awgn, MeasuredRealPower) {
    using namespace dsplib;
    for (int n : {1, 2, 8}) {
        const arr_real x = ones(n) * 2;
        for (real_t snr : {-5, 0, 25}) {
            // MATLAB measured power is mean(abs(x).^2) = 4, even for one sample.
            const auto sigma = 2 * std::pow(10, -snr / 20);
            rng(42);
            const arr_real ref = x + sigma * randn(n);
            rng(42);
            ASSERT_EQ_ARR_REAL(awgn(x, snr), ref);
            auto y = x;
            rng(42);
            awgn(inplace(y), snr);
            ASSERT_EQ_ARR_REAL(y, ref);
        }
    }
}

TEST(Awgn, MeasuredComplexPower) {
    using namespace dsplib;
    for (int n : {1, 2, 8}) {
        const arr_cmplx x = ones(n) * cmplx_t(2, -3);
        for (real_t snr : {-5, 0, 25}) {
            // Total power is 13; each noise component gets half the noise power.
            const auto sigma = std::sqrt(13.0 / 2) * std::pow(10, -snr / 20);
            rng(42);
            const auto noise = randn(2 * n);
            arr_cmplx ref = x;
            for (int i = 0; i < n; ++i) {
                ref[i] += sigma * cmplx_t(noise[2 * i], noise[2 * i + 1]);
            }
            rng(42);
            ASSERT_EQ_ARR_CMPLX(awgn(x, snr), ref);
            auto y = x;
            rng(42);
            awgn(inplace(y), snr);
            ASSERT_EQ_ARR_CMPLX(y, ref);
        }
    }
}

TEST(Awgn, EmptyInput) {
    using namespace dsplib;
    arr_real real_input;
    arr_cmplx complex_input;
    EXPECT_EQ(awgn(real_input, 10).size(), 0);
    EXPECT_EQ(awgn(complex_input, 10).size(), 0);
    awgn(inplace(real_input), 10);
    awgn(inplace(complex_input), 10);
    EXPECT_EQ(real_input.size(), 0);
    EXPECT_EQ(complex_input.size(), 0);
}

//-------------------------------------------------------------------------------------------------
TEST(RandReproduce, Randn) {
    {
        dsplib::rng(42);
        auto x1 = dsplib::randn();
        dsplib::rng(42);
        auto x2 = dsplib::randn();
        ASSERT_EQ(x1, x2);
    }
    {
        dsplib::rng(42);
        auto x1 = dsplib::randn(100);
        dsplib::rng(42);
        auto x2 = dsplib::randn(100);
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(RandReproduce, Randi) {
    {
        dsplib::rng(42);
        auto x1 = dsplib::randi({0, 100});
        dsplib::rng(42);
        auto x2 = dsplib::randi({0, 100});
        ASSERT_EQ(x1, x2);
    }
    {
        dsplib::rng(42);
        auto x1 = dsplib::randi(100);
        dsplib::rng(42);
        auto x2 = dsplib::randi(100);
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(RandReproduce, Rand) {
    {
        dsplib::rng(42);
        auto x1 = dsplib::rand();
        dsplib::rng(42);
        auto x2 = dsplib::rand();
        ASSERT_EQ(x1, x2);
    }
    {
        dsplib::rng(42);
        auto x1 = dsplib::rand(100);
        dsplib::rng(42);
        auto x2 = dsplib::rand(100);
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
}