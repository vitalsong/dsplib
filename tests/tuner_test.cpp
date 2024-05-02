#include "tests_common.h"

using namespace dsplib;

TEST(Tuner, Up) {
    const int fs = 16000;
    const real_t f0 = 440;
    const real_t df = 1000;
    auto x = expj(2 * pi * f0 * arange(10 * fs) / fs);
    x = awgn(x, 100);
    {
        auto tuner = Tuner(fs, df);
        auto y = tuner(x);
        auto harm = harm_analyze(y, 8192);
        const auto target_freq = (f0 + df) / fs;
        ASSERT_NEAR(harm.freq, target_freq, (1.0 / fs));
        ASSERT_GE(harm.snr, 90);
    }
}

TEST(Tuner, Down) {
    const int fs = 16000;
    const real_t f0 = 440;
    const real_t df = -1000;
    auto x = expj(2 * pi * f0 * arange(10 * fs) / fs);
    x = awgn(x, 100);
    {
        auto tuner = Tuner(fs, df);
        auto y = tuner(x);
        auto harm = harm_analyze(y, 8192);
        const auto target_freq = 1 + (f0 + df) / fs;
        ASSERT_NEAR(harm.freq, target_freq, (1.0 / fs));
        ASSERT_GE(harm.snr, 90);
    }
}

TEST(Tuner, UpDown) {
    const int fs = 16000;
    const real_t f0 = 440;
    const real_t df = 500;
    auto x = expj(2 * pi * f0 * arange(10 * fs) / fs);
    x = awgn(x, 100);
    {
        auto tuner1 = Tuner(fs, -df);
        auto y = tuner1(x);
        auto tuner2 = Tuner(fs, df);
        y = tuner2(y);
        auto harm = harm_analyze(y, 8192);
        ASSERT_NEAR(harm.freq, (f0 / fs), (1.0 / fs));
        ASSERT_GE(harm.snr, 90);
    }
}
