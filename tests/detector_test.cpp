#include "tests_common.h"
#include <math.h>

//-------------------------------------------------------------------------------------------------
TEST(Detector, Base)
{
    using namespace dsplib;

    const int r = 1;
    const int n = 100;
    auto zch = expj((-pi * r * (range(n) ^ 2)) / n);
    auto dtc = detector(zch, 0.5);
    auto noise = 1i * randn(10000);
    auto x = concatenate(noise, zch, noise, zch, noise);
    x = awgn(x, 0);

    int dtc_count = 0;
    const int M = 16;
    for (size_t i = 0; i < x.size() / M; i++) {
        int t0 = i * M;
        int t1 = (i + 1) * M;
        arr_cmplx sx = x.slice(t0, t1);
        auto dres = dtc.process(sx);
        if (dres.triggered) {
            ++dtc_count;
        }
    }

    ASSERT_EQ(dtc_count, 2);
}
