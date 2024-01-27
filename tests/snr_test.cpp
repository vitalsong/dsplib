#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
static arr_real _gen_test_signal(int len, real_t fs, arr_real harm_freq, arr_real harm_amp, real_t noise_amp = 0) {
    arr_real tt = arange(0, len) / fs;
    arr_real x = zeros(len);
    assert(harm_freq.size() == harm_amp.size());
    for (int i = 0; i < harm_freq.size(); ++i) {
        const auto freq = harm_freq[i];
        const auto amp = harm_amp[i];
        x += amp * sin(2 * pi * freq * tt);
    }
    x += noise_amp * randn(len);
    return x;
}

//-------------------------------------------------------------------------------------------------
TEST(Sinad, Harm) {
    auto x = _gen_test_signal(1000, 1.0, {0.125, 0.25}, {1.0, 0.025});
    auto tg_sinad = pow2db(abs2(1.0) / abs2(0.025));
    auto rs_sinad = sinad(x);
    ASSERT_NEAR(tg_sinad, rs_sinad, 0.1);
}

TEST(Sinad, HarmNoise) {
    auto x = _gen_test_signal(1000, 1.0, {0.125, 0.25}, {1.0, 0.025}, 0.001);
    auto tg_sinad = pow2db(abs2(1.0) / (abs2(0.025) + abs2(0.001)));
    auto rs_sinad = sinad(x);
    ASSERT_NEAR(tg_sinad, rs_sinad, 0.1);
}

//-------------------------------------------------------------------------------------------------
TEST(Sinad, HarmNoisePxx) {
    auto x = _gen_test_signal(160000, 1.0, {0.125, 0}, {1.0, 0}, 0);
    x = awgn(x, -10);
    auto [pxx, f] = welch(x, 1024, (1024 - 128), 1024, SpectrumType::Power);
    auto rs_sinad = sinad(pxx, SinadType::Power);
    ASSERT_NEAR(-10, rs_sinad, 1.0);
}

//-------------------------------------------------------------------------------------------------
TEST(Thd, Base) {
    const int fs = 1000;
    arr_real harm_freq = {100, 200, 300};
    arr_real harm_amp = {2.0, 0.01, 0.005};
    auto x = _gen_test_signal(1000, 1000, harm_freq, harm_amp);
    auto target = pow2db((abs2(harm_amp[1]) + abs2(harm_amp[2])) / abs2(harm_amp[0]));
    auto res = thd(x, harm_freq.size());
    ASSERT_NEAR(target, res.value, 0.1);

    auto tg_harmfreq = harm_freq / fs;
    ASSERT_EQ_ARR_REAL(tg_harmfreq, res.harmfreq, 1e-5);

    auto tg_harmpow = mag2db(harm_amp * 0.707);
    ASSERT_EQ_ARR_REAL(tg_harmpow, res.harmpow, 0.1);
}

//-------------------------------------------------------------------------------------------------
TEST(Thd, Aliased) {
    const int fs = 10000;
    real_t f = 2100;
    auto t = arange(0, fs) / fs;
    auto x = tanh(sin(2 * pi * f * t) + 0.1) + 0.001 * randn(t.size());

    {
        auto thd_res = thd(x, 2);
        ASSERT_NEAR(thd_res.value, -29.08, 0.1);
        ASSERT_EQ_ARR_REAL(thd_res.harmfreq * fs, arr_real{2100, 4200}, 1.0);
    }

    {
        auto thd_res = thd(x, 7, true);
        ASSERT_NEAR(thd_res.value, -22.5, 0.1);
        ASSERT_EQ_ARR_REAL(thd_res.harmfreq * fs, arr_real{2100, 4200, 3700, 1600, 500, 2600, 4700}, 1.0);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(Snr, Base) {
    const int fs = 1000;
    arr_real harm_freq = {100, 200, 300};
    arr_real harm_amp = {2.0, 0.01, 0.005};
    real_t noise_amp = 0.1;
    auto varnoise = abs2(noise_amp);
    auto powfund = abs2(harm_amp[0]) / 2;
    auto x = _gen_test_signal(fs, fs, harm_freq, harm_amp, noise_amp);
    auto target = pow2db(powfund / varnoise);
    auto res = snr(x, harm_freq.size());
    ASSERT_NEAR(target, res, 1.5);
}

//-------------------------------------------------------------------------------------------------
TEST(Snr, Compare) {
    const int fs = 48000;
    arr_real harm_freq = {1000, 2000};
    arr_real harm_amp = {1.0, 0.4};
    real_t noise_amp = 0.1;
    real_t varnoise = abs2(noise_amp);
    real_t powfund = abs2(harm_amp[0]) / 2;
    real_t powharm = abs2(harm_amp[1]) / 2;
    auto x = _gen_test_signal(fs, fs, harm_freq, harm_amp, noise_amp);

    auto thd_v = thd(x).value;
    auto thd_def = pow2db(powharm / powfund);
    ASSERT_NEAR(thd_v, thd_def, 0.1);

    auto snr_v = snr(x);
    auto snr_def = pow2db(powfund / varnoise);
    ASSERT_NEAR(snr_v, snr_def, 1.5);

    auto sinad_v = sinad(x);
    auto sinad_def = pow2db(powfund / (powharm + varnoise));
    ASSERT_NEAR(sinad_v, sinad_def, 0.1);
}
