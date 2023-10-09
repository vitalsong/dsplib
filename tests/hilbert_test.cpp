#include "dsplib/types.h"
#include "tests_common.h"
#include <gtest/gtest.h>
#include <utility>

using namespace dsplib;

namespace {

struct Harm
{
    real_t freq{0};
    real_t amp{0};
    real_t snr{0};
};

arr_real calc_psd(const arr_real& x) {
    const int n = 1L << nextpow2(x.size());
    auto w = window::kaiser(x.size(), 38);
    w /= rms(w);
    const real_t u = real_t(x.size()) * n / 2;
    auto X = fft(x * w, n);
    X.slice(n / 2 + 1, n) = 0;
    const arr_real spec = abs2(X) / u;
    return spec;
}

arr_real calc_psd(const arr_cmplx& x) {
    const int n = 1L << nextpow2(x.size());
    auto w = window::kaiser(x.size(), 38);
    w /= rms(w);
    const real_t u = real_t(x.size()) * n;
    const auto X = fft(x * w, n);
    const arr_real spec = abs2(X) / u;
    return spec;
}

template<typename T>
Harm harm_analyze(const base_array<T>& x) {
    auto spec = calc_psd(x);
    const auto [pks, locs, wds] = findpeaks(spec, 1);
    Harm res;
    res.amp = pow2db(pks[0]);
    res.freq = locs[0] / spec.size();
    const int lp = std::round(locs[0] - wds[0] / 2);
    const int rp = std::round(locs[0] + wds[0] / 2);
    auto fund_pow = sum(*spec.slice(lp, rp));
    res.snr = pow2db(fund_pow / (sum(spec) - fund_pow + eps()));
    return res;
}

}   // namespace

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
    ASSERT_NEAR(h2.freq, h2.freq, 1e-5);
    ASSERT_NEAR(h2.snr, snr, 6);
}

//-------------------------------------------------------------------------------------------------
TEST(Hilbert, Function) {
    const int fs = 16000;
    const real_t snr = 70;
    const real_t f0 = 0.1;
    const real_t tw = 100.0 / fs;
    auto x1 = sin(2 * pi * f0 * arange(fs));
    x1 = awgn(x1, snr);
    auto x2 = hilbert(x1);
    auto rms_1 = mag2db(rms(x1));
    auto rms_2 = mag2db(rms(x2));
    ASSERT_NEAR(rms_2, (rms_1 + 3.01), 0.1);
    auto h1 = harm_analyze(x1);
    auto h2 = harm_analyze(x2);
    ASSERT_NEAR(h2.freq, h2.freq, 1e-5);
    ASSERT_NEAR(h2.snr, snr, 6);
}