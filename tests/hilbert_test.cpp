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
    {
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
    {
        const int fs = 16000;
        const real_t snr = 70;
        const real_t f0 = 0.1;
        auto x1 = sin(2 * pi * f0 * arange(fs));
        x1 = awgn(x1, snr);
        auto x2 = hilbert(x1, 8192);
        ASSERT_EQ(x2.size(), 8192);
        auto rms_1 = mag2db(rms(x1));
        auto rms_2 = mag2db(rms(x2));
        ASSERT_NEAR(rms_2, (rms_1 + 3.01), 0.1);
        auto h1 = harm_analyze(x1);
        auto h2 = harm_analyze(x2);
        ASSERT_NEAR(h1.freq, h2.freq, 1e-5);
        ASSERT_NEAR(h2.snr, snr, 6);
    }
}

TEST(Hilbert, Constant) {
    for (int n : {3, 4, 7, 8}) {
        const arr_real x = ones(n) * 3;
        ASSERT_EQ_ARR_CMPLX(hilbert(x), complex(x));
    }
}

TEST(Hilbert, Nyquist) {
    for (int n : {4, 8}) {
        const arr_real x = cos(pi * arange(n));
        ASSERT_EQ_ARR_CMPLX(hilbert(x), complex(x));
    }
}

TEST(Hilbert, MixedComponents) {
    for (int n : {5, 6, 7, 8}) {
        // The highest positive-frequency bin must also be doubled for odd n.
        const auto phase = 2 * pi * ((n - 1) / 2) * arange(n) / n;
        arr_real x = 3 + 0.7 * cos(phase);
        arr_cmplx ref = 3 + 0.7 * expj(phase);
        if (n % 2 == 0) {
            const auto nyquist = 1.2 * cos(pi * arange(n));
            x += nyquist;
            ref += nyquist;
        }
        const auto y = hilbert(x);
        ASSERT_EQ_ARR_CMPLX(y, ref);
        ASSERT_EQ_ARR_REAL(real(y), x);
    }
}

TEST(Hilbert, ShortInput) {
    ASSERT_EQ(hilbert(arr_real{}).size(), 0);
    ASSERT_EQ_ARR_CMPLX(hilbert(arr_real{3}), arr_cmplx{3});
    ASSERT_EQ_ARR_CMPLX(hilbert(arr_real{1, 3}), arr_cmplx{1, 3});
}

TEST(Hilbert, ExplicitLength) {
    const arr_real x = {1, 2};
    ASSERT_EQ_ARR_CMPLX(hilbert(x, 4), arr_cmplx{1 - 1i, 2 + 0.5i, 1i, -0.5i});
    const arr_real longer = {1, 2, 3, 4, 100};
    ASSERT_EQ_ARR_CMPLX(hilbert(longer, 4), arr_cmplx{1 + 1i, 2 - 1i, 3 - 1i, 4 + 1i});
}
