#include "dsplib/awgn.h"
#include "dsplib/keywords.h"
#include "dsplib/types.h"
#include "dsplib/utils.h"
#include "dsplib/window.h"
#include "tests_common.h"
#include "dsplib/spectrum.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(Spectrum, RealWelch) {
    const int nfft = 512;
    const int T = 10;
    const int fs = 16000;
    const real_t Ts = 1.0 / fs;
    const auto t = dsplib::arange(fs * T);
    auto x = sin(2 * pi * 440 * t * Ts);
    x = awgn(x, 90);
    {
        auto [pxx, f] = welch(x, nfft, SpectrumType::Psd);
        ASSERT_EQ(pxx.size(), nfft / 2 + 1);
        ASSERT_EQ(f.size(), pxx.size());
        ASSERT_EQ(f[0], 0);
        ASSERT_EQ(f[-1], 0.5);
        ASSERT_NEAR(max(pxx), 185.69, 1e-2);
    }
    {
        auto [sxx, f] = welch(x, nfft, SpectrumType::Power);
        ASSERT_EQ(sxx.size(), nfft / 2 + 1);
        ASSERT_EQ(f.size(), sxx.size());
        ASSERT_EQ(f[0], 0);
        ASSERT_EQ(f[-1], 0.5);
        ASSERT_NEAR(pow2db(max(sxx)), -3.05, 1e-2);
    }
    {
        auto [sxx, f] = welch(x, window::hamming(200), 64, nfft, SpectrumType::Power);
        ASSERT_EQ(sxx.size(), nfft / 2 + 1);
        ASSERT_EQ(f.size(), sxx.size());
        ASSERT_EQ(f[0], 0);
        ASSERT_EQ(f[-1], 0.5);
        ASSERT_NEAR(pow2db(max(sxx)), -3.017, 1e-3);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(Spectrum, CmplxWelch) {
    const int nfft = 512;
    const int T = 10;
    const int fs = 16000;
    const real_t Ts = 1.0 / fs;
    const auto t = dsplib::arange(fs * T);
    auto x = expj(2 * pi * 440 * t * Ts);
    x = awgn(x, 90);
    {
        auto [pxx, f] = welch(x, nfft, SpectrumType::Psd);
        ASSERT_EQ(pxx.size(), nfft);
        ASSERT_EQ(f.size(), pxx.size());
        ASSERT_EQ(f[f.size() / 2 - 1], 0);
        ASSERT_EQ(f[-1], 0.5);
        ASSERT_NEAR(max(pxx), 371.38, 1e-2);
    }
    {
        auto [sxx, f] = welch(x, nfft, SpectrumType::Power);
        ASSERT_EQ(sxx.size(), nfft);
        ASSERT_EQ(f.size(), sxx.size());
        ASSERT_EQ(f[f.size() / 2 - 1], 0);
        ASSERT_EQ(f[-1], 0.5);
        ASSERT_NEAR(pow2db(max(sxx)), -0.043, 1e-2);
    }
}
