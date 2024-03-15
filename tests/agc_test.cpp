#include "tests_common.h"

#include <dsplib.h>

namespace {

bool _anynan(const dsplib::arr_real& x) {
    for (const auto& i : x) {
        if (std::isnan(i)) {
            return true;
        }
    }
    return false;
}

bool _anyinf(const dsplib::arr_real& x) {
    for (const auto& i : x) {
        if (std::isinf(i)) {
            return true;
        }
    }
    return false;
}

}   // namespace
//-------------------------------------------------------------------------------------------------
TEST(AgcTest, TargetLevel) {
    const int fs = 8000;
    auto in = dsplib::sin(2 * dsplib::pi * 1000 * dsplib::arange(2 * fs) / fs);
    in = dsplib::awgn(in, 70);

    const auto target_power = 10;
    const auto max_gain = 20;
    auto agc = dsplib::Agc(dsplib::db2pow(target_power), max_gain);
    auto [out, gain] = agc(in);

    auto amp_rate = dsplib::max(dsplib::abs(in)) / dsplib::max(dsplib::abs(out));
    amp_rate = std::abs(dsplib::mag2db(amp_rate));
    ASSERT_LE(amp_rate, max_gain);

    auto power = dsplib::mag2db(dsplib::rms(*out.slice(fs / 4, dsplib::indexing::end)));
    ASSERT_NEAR(power, target_power, 1.0);
}

//-------------------------------------------------------------------------------------------------
TEST(AgcTest, ValidOnZeros) {
    const int fs = 8000;
    auto in = dsplib::sin(2 * dsplib::pi * 1000 * dsplib::arange(fs) / fs);
    in = in | dsplib::zeros(fs);
    auto agc = dsplib::Agc(dsplib::db2pow(-10), 20);
    auto [out, gain] = agc(in);
    ASSERT_FALSE(_anynan(out) || _anyinf(out));
    ASSERT_FALSE(_anynan(gain) || _anyinf(gain));
}