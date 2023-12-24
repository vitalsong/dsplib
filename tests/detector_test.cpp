#include "tests_common.h"
#include <gtest/gtest.h>
#include <vector>

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
static arr_cmplx zadoff_chu(int r, int n) {
    return expj(-pi * r * power(arange(n), 2) / n);
}

//-------------------------------------------------------------------------------------------------
TEST(Detector, SingleDetect) {
    auto ref = zadoff_chu(1, 199);
    auto dtc = Detector(ref, 0.5);
    auto noise = randn(1000) * 0.1 * rms(ref);
    auto in = noise | awgn(ref, 10) | noise;
    auto state = dtc.process(in);
    ASSERT_EQ(state.out.size(), in.size());
    ASSERT_TRUE(state.triggered);
    arr_cmplx dref = state.out.slice(state.position, state.position + ref.size());
    auto offset = finddelay(ref, dref);
    ASSERT_NEAR(offset, 0, 2);
}

//-------------------------------------------------------------------------------------------------
TEST(Detector, FlowDetect) {
    auto ref = zadoff_chu(1, 255);
    auto dtc = Detector(ref, 0.5);
    auto noise = randn(1000) * 0.1 * rms(ref);
    auto in = noise | awgn(ref, 20) | noise | awgn(ref, 10) | noise;

    const int M = 16;
    std::vector<int> offsets;
    int time = 0;
    for (size_t i = 0; i < in.size() / M; i++) {
        int t0 = i * M;
        int t1 = (i + 1) * M;
        arr_cmplx x = in.slice(t0, t1);
        auto state = dtc.process(x);
        time += x.size();
        if (state.triggered) {
            offsets.push_back(time - x.size() + state.position);
        }
    }

    ASSERT_EQ(offsets.size(), 2);
    ASSERT_NEAR(offsets[0] - dtc.delay_len(), noise.size(), 2);
    ASSERT_NEAR(offsets[1] - dtc.delay_len(), 2 * noise.size() + ref.size(), 2);
}
