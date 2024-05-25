#include "tests_common.h"

#include <cassert>
#include <gtest/gtest.h>

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(Audio, Compressor) {
    const int fs = 16000;
    auto comp = Compressor(fs, -10, 5);

    auto tt = arange(fs) / fs;
    auto x1 = db2mag(-15.0) * sin(2 * pi * 5 * tt);
    auto x2 = db2mag(-6.0) * sin(2 * pi * 5 * tt);

    auto x = concatenate(zeros(1000), x1, x2, zeros(1000));
    auto y = comp(x).out;

    const int t0 = 0;
    const int t1 = 1000;
    const int t2 = t1 + x1.size();
    const int t3 = t2 + x2.size();

    //first section is not changed
    {
        arr_real y1 = y.slice(t1, t2);
        ASSERT_EQ_ARR_REAL(y1, x1);
    }

    //check compressed
    {
        const int nfft = 8192;

        //original
        auto sp1 = welch(x2, window::hamming(nfft), nfft - 128, nfft).pxx;
        auto td1 = thd(sp1, 3, false, SinadType::Power);
        auto thd1 = db2mag(td1.value) * 100;
        auto freq1 = td1.harmfreq[0] * fs;

        //compressed
        arr_real y2 = y.slice(t2, t3);
        auto sp2 = welch(y2, window::hamming(nfft), nfft - 128, nfft).pxx;
        auto td2 = thd(sp2, 3, false, SinadType::Power);
        auto thd2 = db2mag(td2.value) * 100;
        auto freq2 = td2.harmfreq[0] * fs;

        //thd increased
        ASSERT_TRUE(thd2 > thd1);

        //base freq is stable
        ASSERT_NEAR(freq1, freq2, (fs / nfft));
    }
}

TEST(Audio, CompressorDumb) {
    auto comp = Compressor(16000, -10, 5, 0, 0, 0);
    for (int i = 0; i < 100000; ++i) {
        auto [s, g] = comp(randn(512));
        ASSERT_FALSE(anyinf(s) || anynan(s));
        ASSERT_FALSE(anyinf(g) || anynan(g));
    }
}

//-------------------------------------------------------------------------------------------------
TEST(Audio, Limiter) {
    const int fs = 16000;
    real_t threshold = -10;
    auto limiter = Limiter(fs, threshold, 0, 0, 0);

    auto tt = arange(fs) / fs;
    auto x1 = db2mag(-15.0) * sin(2 * pi * 5 * tt);
    auto x2 = db2mag(-6.0) * sin(2 * pi * 5 * tt);

    auto x = concatenate(zeros(1000), x1, x2, zeros(1000));
    auto y = limiter(x).out;

    const int t0 = 0;
    const int t1 = 1000;
    const int t2 = t1 + x1.size();

    //first section is not changed
    arr_real y1 = y.slice(t1, t2);
    ASSERT_EQ_ARR_REAL(y1, x1);

    //check signal limited
    ASSERT_LE(max(abs(y)), db2mag(threshold));
}

TEST(Audio, LimiterDumb) {
    auto limiter = Limiter(16000, -10, 5);
    for (int i = 0; i < 100000; ++i) {
        auto [s, g] = limiter(randn(512));
        ASSERT_FALSE(anyinf(s) || anynan(s));
        ASSERT_FALSE(anyinf(g) || anynan(g));
    }
}

//-------------------------------------------------------------------------------------------------
TEST(Audio, NoiseGate) {
    const int fs = 16000;
    const int M = 3000;

    auto noise_gate = NoiseGate(fs, -15, 0.01, 0.0001, 0.05);

    auto x = sin(2 * pi * 999 * arange(M) / fs);
    x = concatenate(zeros(M), x, zeros(M));
    auto noise = db2mag(-20) * rand({-1, 1}, x.size());
    x += noise;

    const auto y = noise_gate(x).out;

    ASSERT_FALSE(anyinf(y) || anynan(y));

    const int t0 = 0;
    const int t1 = M;
    const int t2 = 2 * M;
    const int t3 = 3 * M;

    //noise was suppressed
    {
        auto rms1 = mag2db(rms(y.slice(t0, t1)) + eps());
        ASSERT_LE(rms1, -70);

        const int dp = 0.05 * fs;
        auto rms2 = mag2db(rms(y.slice(t2 + dp, t3)));
        ASSERT_LE(rms2, -70);
    }

    //signal section don't changed
    {
        auto sinad1 = sinad(x.slice(t1, t2));
        auto sinad2 = sinad(y.slice(t1, t2));
        ASSERT_NEAR(sinad1, sinad2, 0.1);
    }
}