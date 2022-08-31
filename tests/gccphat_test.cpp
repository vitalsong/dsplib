#include "tests_common.h"
#include <dsplib/gccphat.h>

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(GCCPHAT, OneSignal) {
    for (size_t i = 0; i < 1000; i++) {
        int dl = round(20 * randn());
        auto ref = randn(512);
        auto sig = delayseq(ref, dl);
        ref = awgn(ref, 30);
        sig = awgn(sig, 10);
        auto [tau, R] = gccphat(sig, ref);
        ASSERT_NEAR(tau, dl, 0.1);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(GCCPHAT, TwoSignal) {
    for (size_t i = 0; i < 1000; i++) {
        arr_real dls = round(20 * randn(2));
        auto ref = randn(512);
        auto x1 = delayseq(ref, dls[0]);
        auto x2 = delayseq(ref, dls[1]);
        ref = awgn(ref, 30);
        x1 = awgn(x1, 10);
        x2 = awgn(x2, 10);
        auto [tau, R] = gccphat({x1, x2}, ref);
        ASSERT_EQ_ARR_REAL(tau, dls, 0.1);
    }
}