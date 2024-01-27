#include "tests_common.h"
#include <gtest/gtest.h>
#include <vector>

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
static arr_cmplx zadoff_chu(int r, int n) {
    return expj(-pi * r * power(arange(n), 2) / n);
}

//-------------------------------------------------------------------------------------------------
TEST(PreambleDetector, SingleDetect) {
    auto ref = zadoff_chu(1, 199);
    auto dtc = PreambleDetector(ref, 0.5);
    auto noise = randn(1000) * 0.1 * rms(ref);
    auto in = noise | awgn(ref, 10) | noise;
    const int nn = in.size() / dtc.frame_len() * dtc.frame_len();
    in = in.slice(0, nn);

    auto result = dtc.process(in);
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->preamble.size(), ref.size());
    ASSERT_EQ(finddelay(ref, result->preamble), 0);
}

//-------------------------------------------------------------------------------------------------
TEST(PreambleDetector, FlowDetect) {
    auto ref = zadoff_chu(1, 255);
    auto dtc = PreambleDetector(ref, 0.5);
    auto noise = randn(10000) * 0.1 * rms(ref);
    auto in = noise | awgn(ref, 20) | noise | awgn(ref, 10) | noise;

    int detected = 0;
    const int frame_len = dtc.frame_len();
    const int L = in.size() / frame_len;
    for (size_t i = 0; i < L; i++) {
        int t0 = i * frame_len;
        int t1 = t0 + frame_len;
        arr_cmplx x = in.slice(t0, t1);
        auto result = dtc.process(x);
        if (result.has_value()) {
            detected++;
            ASSERT_EQ(finddelay(ref, result->preamble), 0);
        }
    }
    ASSERT_EQ(detected, 2);
}
