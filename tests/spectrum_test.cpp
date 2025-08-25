#include "tests_common.h"

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

//-------------------------------------------------------------------------------------------------
TEST(STFT, Sin) {
    const int nfft = 512;
    const int T = 1;
    const int fs = 16000;
    const auto t = arange(fs * T) / fs;
    auto x = sin(2 * pi * 1000 * t);

    const auto win = window::hann(nfft, false);
    const int overlap = 3 * nfft / 4;
    ASSERT_TRUE(iscola(win, overlap));

    const auto Yc = stft(x, win, overlap, nfft, StftRange::Centered);
    {
        ASSERT_EQ(Yc[0].size(), nfft);
        ASSERT_NEAR(abs(Yc[0][223]), 128, 1e-5);
        ASSERT_NEAR(abs(Yc[0][287]), 128, 1e-5);
        ASSERT_NEAR(abs(Yc[Yc.size() - 1][223]), 128, 1e-5);
        ASSERT_NEAR(abs(Yc[Yc.size() - 1][287]), 128, 1e-5);
    }

    const auto Yo = stft(x, win, overlap, nfft, StftRange::Onesided);
    {
        ASSERT_EQ(Yo[0].size(), nfft / 2 + 1);
        ASSERT_NEAR(abs(Yo[0][32]), 128, 1e-5);
        ASSERT_NEAR(abs(Yo[Yo.size() - 1][32]), 128, 1e-5);
    }

    const auto Yt = stft(x, win, overlap, nfft, StftRange::Twosided);
    {
        ASSERT_EQ(Yt[0].size(), nfft);
        ASSERT_NEAR(abs(Yt[0][32]), 128, 1e-5);
        ASSERT_NEAR(abs(Yt[0][480]), 128, 1e-5);
        ASSERT_NEAR(abs(Yt[Yt.size() - 1][32]), 128, 1e-5);
        ASSERT_NEAR(abs(Yt[Yt.size() - 1][480]), 128, 1e-5);
    }

    const auto xr = istft(Yc, win, overlap, nfft, StftRange::Centered);
    ASSERT_EQ(x.size(), xr.size());

    //ignore first zero element
    auto err = abs(x - xr).slice(1, indexing::end).copy();
    auto max_err = max(abs(err));
    ASSERT_LE(max_err, 1e-8);
}

//-------------------------------------------------------------------------------------------------
TEST(STFT, Chirp) {
    const int nfft = 512;
    const int T = 1;
    const int fs = 16000;
    const real_t Ts = 1.0 / fs;
    auto x = chirp(fs * T, 20, fs / 2, fs);
    x = awgn(x, 90);

    const auto win = window::hann(nfft, false);
    const int overlap = 3 * nfft / 4;
    ASSERT_TRUE(iscola(win, overlap));

    for (auto range : {StftRange::Centered, StftRange::Onesided, StftRange::Twosided}) {
        for (auto method : {OverlapMethod::Ola, OverlapMethod::Wola}) {
            const auto Y = stft(x, win, overlap, nfft, range);
            const auto xr = istft(Y, win, overlap, nfft, range, method);
            ASSERT_EQ(x.size(), xr.size());
            auto err = abs(x - xr).slice(1, indexing::end).copy();   //ignore first zero element
            auto max_err = max(abs(err));
            ASSERT_LE(max_err, 1e-8);
        }
    }
}

//-------------------------------------------------------------------------------------------------
TEST(Coherence, Real) {
    const int fs = 16000;
    auto x = randn(fs * 10);
    const real_t wn = 0.1;
    const int winlen = 512;
    const auto h = fir1(300, wn);
    const auto y = FirFilterR(h).process(x);

    const auto dl = finddelay(x, y);
    x = delayseq(x, dl + 3);

    const int tp1 = winlen * (wn * 0.45);
    const int tp2 = winlen * (wn * 0.6);

    {
        const auto coh = mscohere(x, y, winlen);
        ASSERT_NEAR(mean(coh.slice(0, tp1)), 1.0, 1e-2);
        ASSERT_NEAR(mean(coh.slice(tp2, indexing::end)), 0.0, 1e-2);
    }

    {
        const auto ny = awgn(y, 0);
        const auto coh = mscohere(x, ny, winlen);
        ASSERT_NEAR(mean(coh.slice(0, tp1)), 0.91, 1e-2);
        ASSERT_NEAR(mean(coh.slice(tp2, indexing::end)), 0.0, 1e-2);
    }

    {
        const auto ny = awgn(y, -10);
        const auto coh = mscohere(x, ny, winlen);
        ASSERT_NEAR(mean(coh.slice(0, tp1)), 0.5, 1e-2);
        ASSERT_NEAR(mean(coh.slice(tp2, indexing::end)), 0.0, 1e-2);
    }
}