#include "tests_common.h"

//-------------------------------------------------------------------------------------------------
static dsplib::arr_cmplx zadoff_chu(int r, int n) {
    return dsplib::expj((-dsplib::pi * r * (dsplib::range(n) ^ 2)) / n);
}

//-------------------------------------------------------------------------------------------------
TEST(Detector, SingleDetect) {
    using namespace dsplib;
    auto zch = zadoff_chu(1, 201) | zadoff_chu(1, 101);
    auto dtc = detector(zch, 0.5);
    auto noise = 1i * randn(10000);
    auto x = noise | zch | noise;
    x = awgn(x, 0);
    auto dres = dtc.process(x);
    ASSERT_TRUE(dres.triggered);
    ASSERT_NEAR(dres.delay.size(), zch.size() + noise.size(), 2);
}

//-------------------------------------------------------------------------------------------------
TEST(Detector, FlowDetect) {
    using namespace dsplib;
    auto zch = zadoff_chu(1, 201) | zadoff_chu(1, 101);
    auto dtc = detector(zch, 0.5);
    auto noise = 1i * randn(10000);
    auto x = noise | zch | noise;
    x = awgn(x, 0);

    int dtc_count = 0;
    const int M = 16;
    int spent_time = 0;
    for (size_t i = 0; i < x.size() / M; i++) {
        int t0 = i * M;
        int t1 = (i + 1) * M;
        arr_cmplx sx = x.slice(t0, t1);
        spent_time += sx.size();
        auto dres = dtc.process(sx);
        if (dres.triggered) {
            ++dtc_count;
            spent_time -= dres.delay.size();
            ASSERT_NEAR(spent_time, noise.size(), 2);
        }
    }

    ASSERT_EQ(dtc_count, 1);
}
