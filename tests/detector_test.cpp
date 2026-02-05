#include "tests_common.h"
#include <gtest/gtest.h>

using namespace dsplib;

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

    ASSERT_ANY_THROW({
        auto x = complex(zeros(dtc.frame_len() + 1));
        auto r = dtc.process(x);
    });
}

//-------------------------------------------------------------------------------------------------
TEST(PreambleDetector, FlowDetect) {
    auto ref = zadoff_chu(1, 255);
    auto dtc = PreambleDetector(ref, 0.5);
    auto noise = randn(10000) * 0.1 * rms(ref);
    auto in = noise | awgn(ref, 20) | noise | awgn(ref, 10) | noise;

    int detected = 0;
    const int frame_len = dtc.frame_len();
    ChunkBuffer<cmplx_t> buf(frame_len);
    buf.write(in, [&](auto x) {
        auto result = dtc.process(x);
        if (result.has_value()) {
            detected++;
            ASSERT_EQ(finddelay(ref, result->preamble), 0);
        }
    });

    ASSERT_EQ(detected, 2);
}
