#include <gtest/gtest.h>
#include "tests_common.h"

namespace {

void _check_equal(dsplib::arr_real x1, dsplib::arr_real x2, int fs1 = 1, int fs2 = 1) {
    auto thd1 = dsplib::thd(x1);
    auto thd2 = dsplib::thd(x2);

    //freq not changed
    const int freq1 = thd1.harmfreq[0] * fs1;
    const int freq2 = thd2.harmfreq[0] * fs2;
    ASSERT_NEAR(freq1, freq2, 1.0);

    //аmp not changed
    auto pow_delta = dsplib::abs(thd1.harmpow[0] - thd2.harmpow[0]);
    ASSERT_LE(pow_delta, 0.5);

    //no distortion
    const auto sinad1 = dsplib::sinad(x1);
    const auto sinad2 = dsplib::sinad(x2);
    ASSERT_NEAR(sinad1, sinad2, 6.0);
}

void _check_phase(dsplib::arr_real x1, dsplib::arr_real x2) {
    using namespace dsplib;
    arr_cmplx y1 = fft(x1).slice(0, x1.size() / 2 + 1);
    arr_cmplx y2 = fft(x2).slice(0, x2.size() / 2 + 1);
    auto p1 = max(y1);
    auto p2 = max(y2);
    auto df = rad2deg(angle(p1 * conj(p2)));
    ASSERT_LE(df, 1.0);
}

}   // namespace

TEST(Resampler, Interp) {
    constexpr int L = 3;
    constexpr int fs = 16000;
    dsplib::FIRInterpolator interp(L);

    const int frame_len = 512;
    const int num_frames = 100;
    auto tt = dsplib::arange(num_frames * frame_len);
    auto x_in = dsplib::sin(2 * dsplib::pi * 0.4 * tt);
    x_in = dsplib::awgn(x_in, 50);

    dsplib::arr_real x_out;
    for (int i = 0; i < num_frames; ++i) {
        dsplib::arr_real in = x_in.slice(i * frame_len, (i + 1) * frame_len);
        auto out = interp.process(in);
        ASSERT_EQ(out.size(), in.size() * L);
        x_out |= dsplib::arr_real(out);
    }

    const int N = 8192;
    const int dl = interp.delay();
    dsplib::arr_real x1 = x_in.slice(0, N);
    dsplib::arr_real x2 = x_out.slice(dl, dl + N);

    _check_equal(x1, x2, fs, fs * L);
}

TEST(Resampler, Decim) {
    constexpr int D = 2;
    constexpr int fs = 32000;
    dsplib::FIRDecimator decim(D);

    const int frame_len = decim.prev_size(512);
    const int num_frames = 100;
    auto tt = dsplib::arange(num_frames * frame_len);
    auto x_in = dsplib::sin(2 * dsplib::pi * 0.2 * tt);
    x_in = dsplib::awgn(x_in, 50);

    dsplib::arr_real x_out;
    for (int i = 0; i < num_frames; ++i) {
        dsplib::arr_real in = x_in.slice(i * frame_len, (i + 1) * frame_len);
        auto decim_out = decim.process(in);
        ASSERT_EQ(decim_out.size(), in.size() / D);
        x_out |= dsplib::arr_real(decim_out);
    }

    const int N = 8192;
    const int dl = decim.delay();
    dsplib::arr_real x1 = x_in.slice(0, N);
    dsplib::arr_real x2 = x_out.slice(dl, dl + N);

    _check_equal(x1, x2, fs, fs / D);
}

TEST(Resampler, Resamp) {
    const int fs1 = 44100;
    const int fs2 = 48000;
    dsplib::FIRResampler resamp(fs2, fs1);

    const int frame_len = resamp.prev_size(512);
    const int num_frames = 100;
    auto tt = dsplib::arange(num_frames * frame_len);
    auto x_in = dsplib::sin(2 * dsplib::pi * 0.4 * tt);
    x_in = dsplib::awgn(x_in, 50);
    dsplib::arr_real x_out;

    for (int i = 0; i < num_frames; ++i) {
        dsplib::arr_real in = x_in.slice(i * frame_len, (i + 1) * frame_len);
        auto decim_out = resamp.process(in);
        x_out |= dsplib::arr_real(decim_out);
    }

    const int N = 8192;
    const int dl = resamp.delay();
    dsplib::arr_real x1 = x_in.slice(0, N);
    dsplib::arr_real x2 = x_out.slice(dl, dl + N);

    _check_equal(x1, x2, fs1, fs2);
}

TEST(Resampler, ResampleFunc) {
    using namespace dsplib;

    {
        //interpolation
        const int fs1 = 16000;
        const int fs2 = 32000;
        auto x_in = sin(2 * pi * 0.4 * arange(fs1));
        x_in = awgn(x_in, 50);
        auto x_out = resample(x_in, fs2, fs1);
        ASSERT_EQ(x_out.size(), fs2);

        const int N = 8192;
        arr_real x1 = x_in.slice(0, N);
        arr_real x2 = x_out.slice(0, N);
        _check_equal(x1, x2, fs1, fs2);
        _check_phase(x_in, x_out);
    }

    {
        //decimation
        const int fs1 = 32000;
        const int fs2 = 16000;
        auto x_in = sin(2 * pi * 0.2 * arange(fs1));
        x_in = awgn(x_in, 50);
        auto x_out = resample(x_in, fs2, fs1);
        ASSERT_EQ(x_out.size(), fs2);

        const int N = 8192;
        arr_real x1 = x_in.slice(0, N);
        arr_real x2 = x_out.slice(0, N);
        _check_equal(x1, x2, fs1, fs2);
        _check_phase(x_in, x_out);
    }

    {
        //rate convert (up)
        const int fs1 = 44100;
        const int fs2 = 48000;
        auto x_in = sin(2 * pi * 0.4 * arange(fs1));
        x_in = awgn(x_in, 50);
        auto x_out = resample(x_in, fs2, fs1);
        ASSERT_EQ(x_out.size(), fs2);

        const int N = 8192;
        arr_real x1 = x_in.slice(0, N);
        arr_real x2 = x_out.slice(0, N);
        _check_equal(x1, x2, fs1, fs2);
        _check_phase(x_in, x_out);
    }

    {
        //rate convert (down)
        const int fs1 = 44100;
        const int fs2 = 16000;
        auto x_in = sin(2 * pi * 0.05 * arange(fs1));
        x_in = awgn(x_in, 50);
        auto x_out = resample(x_in, fs2, fs1);
        ASSERT_EQ(x_out.size(), fs2);

        const int N = 8192;
        arr_real x1 = x_in.slice(0, N);
        arr_real x2 = x_out.slice(0, N);
        _check_equal(x1, x2, fs1, fs2);
        _check_phase(x_in, x_out);
    }

    {
        //not multiple input size
        const int q = 3;
        const int fs2 = 16000;
        const int fs1 = fs2 * q;
        auto x_in = sin(2 * pi * 0.05 * arange(fs1 - (q - 1)));
        auto x_out = resample(x_in, fs2, fs1);
        ASSERT_EQ(x_out.size(), fs2);
    }
}

//TODO: check FIRDecimator equal FIRRateConverter

TEST(Resampler, ChirpInterpDecim) {
    using namespace dsplib;

    const int fs1 = 16000;
    const int fs2 = 32000;
    const int n = std::pow(2, nextpow2(fs1));
    const int nn = 8 * n;

    auto x = chirp(n, 0, (fs1 / 2), fs1);
    auto y1 = resample(x, fs2, fs1);
    auto y2 = resample(y1, fs1, fs2);

    auto X = fft(x, nn);
    auto Y = fft(y2, nn);
    arr_real Z = mag2db(abs(X / Y)).slice(0, nn / 2);

    real_t fstop = 0;
    for (int i = 0; i < Z.size(); ++i) {
        if (Z[i] > 6.0) {
            fstop = real_t(i) / Z.size();
            break;
        }
    }
    ASSERT_GE(fstop, 0.95);
}