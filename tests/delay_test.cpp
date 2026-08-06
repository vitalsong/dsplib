#include "tests_common.h"

#include <dsplib/delay.h>
#include <dsplib/gccphat.h>

//-------------------------------------------------------------------------------------------------
TEST(DelayTest, Base) {
    {
        dsplib::arr_real x = {0, 1, 2, 3, 4, 5};
        dsplib::arr_real y = {0, 0, 1, 2, 3, 4};
        auto delay = dsplib::DelayReal(1);
        ASSERT_EQ_ARR_REAL(y, delay(x));
    }
    {
        dsplib::arr_real x = {1, 2, 3, 4, 5, 6};
        auto delay = dsplib::DelayReal(0);
        ASSERT_EQ_ARR_REAL(x, delay(x));
    }
    {
        const int n = 10000;
        const int delay_len = 100;
        auto x = dsplib::randn(n);
        auto delay = dsplib::DelayReal(delay_len);
        auto y = delay(x);
        auto [tau, _] = dsplib::gccphat(y, x);
        ASSERT_NEAR(tau, delay_len, 0.1);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(DelayTest, Initial) {
    const int n = 10000;
    const int delay_len = 100;
    auto x = dsplib::randn(n);
    auto delay = dsplib::DelayReal(x.slice(0, delay_len));
    auto y = delay(x.slice(delay_len, n));
    y = y | delay(dsplib::zeros(delay_len));
    auto [tau, _] = dsplib::gccphat(y, x);
    ASSERT_NEAR(tau, 0, 0.1);
    ASSERT_EQ_ARR_REAL(x, y);
}