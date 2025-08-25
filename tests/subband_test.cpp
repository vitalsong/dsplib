#include <gtest/gtest.h>

#include <dsplib.h>
#include <memory>

using namespace dsplib;

namespace {

int _align_arrays(arr_real& x1, arr_real& x2) {
    const int delay = finddelay(x1, x2);
    x1 = delayseq(x1, delay);
    return delay;
}

}   // namespace

//Check that the output power of the filter is equivalent to the output of the dsp.Channelizer in matlab.
TEST(Subband, ChannelizerRms) {
    struct ChanInfo
    {
        int nbands;
        int ntaps;
        int decim;
        real_t rms;
    };

    const int fs = 16000;
    auto t = arange(10 * fs) / fs;
    auto x = sin(2 * pi * 1000 * t);

    std::vector<ChanInfo> test_cases = {{8, 12, 2, -12.05}, {32, 8, 4, -12.07}, {64, 8, 1, -21.13}};

    for (auto test : test_cases) {
        const int nbands = test.nbands;
        const int ntaps = test.ntaps;
        const int decim = test.decim;

        auto chan = Channelizer(nbands, decim, ntaps);

        real_t accum = 0;
        int processed = 0;
        ChunkBuffer<real_t> buf(chan.frame_len());
        buf.write(x, [&](auto x) {
            const auto X = chan(x);
            accum += sum(abs2(X));
            processed += x.size();
        });
        accum /= processed;

        auto out_rms = pow2db(accum);

        ASSERT_NEAR(test.rms, out_rms, 0.1);
    }
}

TEST(Subband, Reconstruction) {
    int nbands = 512;
    int ntaps = 2;
    int decim = 2;
    int fs = 16000;

    auto chan = Channelizer(nbands, decim, ntaps);
    auto synth = ChannelSynthesizer(nbands, decim, ntaps);

    auto in = randn(fs * 10);
    arr_real out;

    ChunkBuffer<real_t> buf(chan.frame_len());
    buf.write(in, [&](auto x) {
        const auto X = chan(x);
        auto xx = synth(X);
        out = concatenate(out, xx);
    });

    int delay = _align_arrays(in, out);
    auto x1 = in.slice(-fs, indexing::end);
    auto x2 = out.slice(-fs, indexing::end);
    auto [_, R] = gccphat(x1, x2);
    auto aR = abs(R);
    auto corr_peak = max(aR);
    ASSERT_GE(corr_peak, 0.99);

    //weak correlation side lobes
    auto max_lobes = max(aR.slice(1, indexing::end));
    ASSERT_LE(max_lobes, 0.005);
    //TODO: check auto-corr function
}

TEST(Subband, Oversampled2x) {
    const int nbands = 512;
    const int ntaps = 4;
    const int D = 2;

    const int fs = 16000;

    auto wp = 1.0 / nbands;
    const auto h = fir1((ntaps * nbands - 1), wp);
    const auto hptr = std::make_shared<const arr_real>(h);

    auto chan = Channelizer(hptr, nbands, D);
    auto synth = ChannelSynthesizer(hptr, nbands, D);

    auto in = randn(fs * 10);

    arr_real out;
    ChunkBuffer<real_t> buf(chan.frame_len());
    buf.write(in, [&](auto x) {
        auto X = chan.process(x);
        ASSERT_EQ(X.size(), nbands);
        out |= synth.process(X);
    });

    //compensate delay
    //TODO: add delay() to Channelizer
    int delay = _align_arrays(in, out);
    ASSERT_NEAR(delay, (h.size() - chan.frame_len()), 1);

    //power is equal
    arr_real in_d = in.slice(-fs, indexing::end);
    arr_real out_d = out.slice(-fs, indexing::end);
    auto in_rms = mag2db(rms(in_d));
    auto out_rms = mag2db(rms(out_d));
    ASSERT_NEAR(in_rms, out_rms, 2);

    //xcorr(in, out) must be near
    auto [_, corr] = gccphat(in_d, out_d);
    ASSERT_GE(abs(max(corr)), 0.98);
}
