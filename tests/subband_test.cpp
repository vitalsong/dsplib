#include "dsplib/subband.h"
#include <gtest/gtest.h>

#include <dsplib.h>
#include <memory>

using namespace dsplib;

TEST(Subband, Oversampled2x) {
    const int N = 512;
    const int fs = 16000;
    const int M = 4;
    const int D = 2;

    auto wp = 1.0 / N;
    const auto h = fir1((M * N - 1), wp) * std::sqrt(N / 2);
    const auto hptr = std::make_shared<const arr_real>(h);
    const int frame_len = N / D;
    auto chan = Channelizer(hptr, N, D, M);
    auto synth = ChannelSynthesizer(hptr, N, D, M);

    const int num_frames = (fs * 30) / frame_len;
    auto in = randn(num_frames * frame_len);
    auto out = arr_real();
    for (int k = 0; k < num_frames; ++k) {
        int t1 = k * frame_len;
        int t2 = t1 + frame_len;
        auto X = chan.process(in.slice(t1, t2));
        ASSERT_EQ(X.size(), N);
        out |= synth.process(X);
    }

    //compensate delay
    //TODO: add delay() to Channelizer
    int delay = finddelay(in, out);
    ASSERT_NEAR(delay, (h.size() - frame_len), 1);
    in = delayseq(in, delay);

    arr_real in_d = in.slice(-fs, indexing::end);
    arr_real out_d = out.slice(-fs, indexing::end);
    auto in_rms = mag2db(rms(in_d));
    auto out_rms = mag2db(rms(out_d));

    //power is equal
    ASSERT_NEAR(in_rms, out_rms, 2);

    //xcorr(in, out) must be near
    auto [tau, corr] = gccphat(in_d, out_d);
    ASSERT_GE(abs(max(corr)), 0.98);
}
