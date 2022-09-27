#include "tests_common.h"

//-------------------------------------------------------------------------------------------------
TEST(Awgn, Sinad) {
    {
        const int fs = 8000;
        auto tt = dsplib::range(fs) / fs;
        auto x = 0.1 * dsplib::sin(2 * dsplib::pi * 1200 * tt);
        x = dsplib::awgn(x, 10);
        auto sinad = dsplib::sinad(x);
        ASSERT_NEAR(sinad, 10, 0.5);
    }

    {
        const int fs = 8000;
        auto tt = dsplib::range(fs) / fs;
        auto x = 0.1 * dsplib::sin(2 * dsplib::pi * 1200 * tt);
        x = dsplib::awgn(x, 25);
        auto sinad = dsplib::sinad(x);
        ASSERT_NEAR(sinad, 25, 0.5);
    }
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