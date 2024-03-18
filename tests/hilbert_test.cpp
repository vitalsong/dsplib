#include "dsplib/types.h"
#include "tests_common.h"
#include <gtest/gtest.h>
#include <utility>

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(Hilbert, Filter) {
    const int fs = 16000;
    const real_t snr = 70;
    const real_t f0 = 0.1;
    const real_t tw = 100.0 / fs;
    auto flt = HilbertFilter(101, tw);
    auto x1 = sin(2 * pi * f0 * arange(fs));
    x1 = awgn(x1, snr);
    auto x2 = flt.process(x1);
    auto rms_1 = mag2db(rms(x1));
    auto rms_2 = mag2db(rms(x2));
    ASSERT_NEAR(rms_2, (rms_1 + 3.01), 0.1);
    auto h1 = harm_analyze(x1);
    auto h2 = harm_analyze(x2);
    ASSERT_NEAR(h1.freq, h2.freq, 1e-5);
    ASSERT_NEAR(h2.snr, snr, 6);
}

//-------------------------------------------------------------------------------------------------
TEST(Hilbert, Function) {
    const int fs = 16000;
    const real_t snr = 70;
    const real_t f0 = 0.1;
    auto x1 = sin(2 * pi * f0 * arange(fs));
    x1 = awgn(x1, snr);
    auto x2 = hilbert(x1);
    auto rms_1 = mag2db(rms(x1));
    auto rms_2 = mag2db(rms(x2));
    ASSERT_NEAR(rms_2, (rms_1 + 3.01), 0.1);
    auto h1 = harm_analyze(x1);
    auto h2 = harm_analyze(x2);
    ASSERT_NEAR(h1.freq, h2.freq, 1e-5);
    ASSERT_NEAR(h2.snr, snr, 6);
}