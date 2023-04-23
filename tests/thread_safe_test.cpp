#include "tests_common.h"

#include <cstdlib>
#include <thread>
#include <iostream>
#include <future>
#include <vector>

//-------------------------------------------------------------------------------------------------
TEST(ThreadSafe, FftCmplx) {
    using namespace dsplib;

    std::vector<int> nfft_list = {128, 512, 1024, 4096, 8192};
    int test_steps = 1000;
    while (--test_steps) {
        int idx = std::rand() % nfft_list.size();
        int n = nfft_list[idx];
        auto x1 = dsplib::randn(n);
        auto x2 = x1;

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
