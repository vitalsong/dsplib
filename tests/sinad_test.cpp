#include "tests_common.h"
#include <dsplib/sinad.h>

//-------------------------------------------------------------------------------------------------
TEST(SinadTest, HarmAndNoise) {
    dsplib::arr_real n = dsplib::arange(0, 1000);

    {
        auto baselvl = 1;
        auto harmlvl = 0.025;
        auto powfund = dsplib::pow2(baselvl);
        auto powharm = dsplib::pow2(harmlvl);
        auto thSINAD = 10 * dsplib::log10(powfund / powharm);
        dsplib::arr_real x = baselvl * dsplib::cos(dsplib::pi / 4 * n) + harmlvl * dsplib::sin(dsplib::pi / 2 * n);
        auto rsinad = dsplib::sinad(x);
        ASSERT_NEAR(thSINAD, rsinad, 0.5);
    }

    {
        auto baselvl = 1;
        auto harmlvl = 0.025;
        auto noiselvl = 0.01;
        auto powfund = dsplib::pow2(baselvl);
        auto powharm = dsplib::pow2(harmlvl);
        auto varnoise = dsplib::pow2(noiselvl);
        auto thSINAD = 10 * dsplib::log10(powfund / (powharm + varnoise));
        dsplib::arr_real x = baselvl * dsplib::cos(dsplib::pi / 4 * n) + harmlvl * dsplib::sin(dsplib::pi / 2 * n) +
                             noiselvl * dsplib::randn(n.size());
        auto rsinad = dsplib::sinad(x);
        ASSERT_NEAR(thSINAD, rsinad, 3);
    }
}
