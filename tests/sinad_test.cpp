#include "tests_common.h"
#include <math.h>
#include <dsplib/sinad.h>

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(SinadTest, HarmAndNoise)
{
    arr_real n = range(0, 1000);

    {
        auto baselvl = 1;
        auto harmlvl = 0.025;
        auto powfund = baselvl * baselvl;
        auto powharm = harmlvl * harmlvl;
        auto thSINAD = 10 * log10(powfund / powharm);
        arr_real x = baselvl * dsplib::cos(dsplib::pi / 4 * n) + harmlvl * dsplib::sin(dsplib::pi / 2 * n);
        auto rsinad = dsplib::sinad(x);
        ASSERT_NEAR(thSINAD, rsinad, 0.5);
    }

    {
        auto baselvl = 1;
        auto harmlvl = 0.025;
        auto noiselvl = 0.01;
        auto powfund = baselvl * baselvl;
        auto powharm = harmlvl * harmlvl;
        auto varnoise = noiselvl * noiselvl;
        auto thSINAD = 10 * log10(powfund / (powharm + varnoise));
        arr_real x = baselvl * dsplib::cos(dsplib::pi / 4 * n) + harmlvl * dsplib::sin(dsplib::pi / 2 * n) + noiselvl * dsplib::randn(n.size());
        auto rsinad = dsplib::sinad(x);
        ASSERT_NEAR(thSINAD, rsinad, 3);
    }
}
