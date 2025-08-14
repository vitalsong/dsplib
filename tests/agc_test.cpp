#include "tests_common.h"

#include <dsplib.h>

using namespace dsplib;

TEST(AgcTest, TargetLevelR) {
    const int fs = 8000;
    auto in = sin(2 * pi * 1000 * arange(2 * fs) / fs);
    in = awgn(in, 70);

    const auto target_power = 10;
    const auto max_gain = 20;
    auto agc = Agc(db2pow(target_power), max_gain);
    auto [out, gain] = agc(in);

    auto amp_rate = max(abs(in)) / max(abs(out));
    amp_rate = std::abs(mag2db(amp_rate));
    ASSERT_LE(amp_rate, max_gain);

    auto power = mag2db(rms(out.slice(fs / 4, indexing::end)));
    ASSERT_NEAR(power, target_power, 1.0);
}

TEST(AgcTest, TargetLevelC) {
    const int fs = 8000;
    auto in = expj(2 * pi * 1000 * arange(2 * fs) / fs);
    in = awgn(in, 70);

    const auto target_power = 10;
    const auto max_gain = 20;
    auto agc = Agc(db2pow(target_power), max_gain);
    auto [out, gain] = agc(in);

    auto amp_rate = max(abs(in)) / max(abs(out));
    amp_rate = std::abs(mag2db(amp_rate));
    ASSERT_LE(amp_rate, max_gain);

    auto power = mag2db(rms(out.slice(fs / 4, indexing::end)));
    ASSERT_NEAR(power, target_power, 1.0);
}

TEST(AgcTest, ValidOnZeros) {
    const int fs = 8000;
    auto in = sin(2 * pi * 1000 * arange(fs) / fs);
    in = in | zeros(fs);
    auto agc = Agc(db2pow(-10), 20);
    auto [out, gain] = agc(in);
    ASSERT_FALSE(anynan(out) || anyinf(out));
    ASSERT_FALSE(anynan(gain) || anyinf(gain));
}