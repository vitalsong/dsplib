#include "tests_common.h"

#include <cstdlib>
#include <thread>
#include <iostream>
#include <future>
#include <vector>

//-------------------------------------------------------------------------------------------------
TEST(ThreadSafe, FftCmplx) {
    using namespace dsplib;

    int test_steps = 1000;
    while (--test_steps) {
        const int n = 1L << randi({6, 13});
        arr_cmplx x1 = dsplib::randn(n) + dsplib::randn(n) * 1i;
        arr_cmplx x2 = x1;

        auto f1 = std::async([&]() {
            return dsplib::fft(x1);
        });

        auto f2 = std::async([&]() {
            return dsplib::fft(x2);
        });

        auto y1 = f1.get();
        auto y2 = f2.get();
        ASSERT_EQ_ARR_CMPLX(y1, y2);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(ThreadSafe, FftReal) {
    using namespace dsplib;

    int test_steps = 1000;
    while (--test_steps) {
        const int n = 1L << randi({6, 13});
        arr_real x1 = randn(n);
        arr_real x2 = x1;

        auto f1 = std::async([&]() {
            return dsplib::fft(x1);
        });

        auto f2 = std::async([&]() {
            return dsplib::fft(x2);
        });

        auto y1 = f1.get();
        auto y2 = f2.get();
        ASSERT_EQ_ARR_CMPLX(y1, y2);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(ThreadSafe, FftNonPow2) {
    using namespace dsplib;

    int test_steps = 1000;
    while (--test_steps) {
        const int n = randi({100, 2000});
        arr_cmplx x1 = dsplib::randn(n);
        arr_cmplx x2 = x1;

        auto f1 = std::async([&]() {
            return dsplib::fft(x1);
        });

        auto f2 = std::async([&]() {
            return dsplib::fft(x2);
        });

        auto y1 = f1.get();
        auto y2 = f2.get();
        ASSERT_EQ_ARR_CMPLX(y1, y2);
    }
}