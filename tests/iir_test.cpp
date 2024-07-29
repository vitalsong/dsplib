#include "tests_common.h"
#include <cmath>

using namespace dsplib;

TEST(IIR, SOSFilter) {
    real_t g = 0.00289644590202159;
    std::vector<arr_real> den = {{1, 6.98117778863416e-15, 0.00619395865710930},
                                 {1, -1.15315743065558e-14, 0.0576378105614471},
                                 {1, 3.37230243729891e-15, 0.171572875253811},
                                 {1, -1.33226762955019e-15, 0.375524805944922},
                                 {1, -6.10622663543836e-16, 0.729453817281744}};

    std::vector<arr_real> num = {{1, 2, 1}, {1, 2, 1}, {1, 2, 1}, {1, 2, 1}, {1, 2, 1}};

    SOSFilter flt(num, den, g);

    const int fs = 8000;
    auto t = arange(fs) / fs;
    // auto x = sin(2 * pi * 1000 * t) + sin(2 * pi * 3000 * t) + 0.01 * randn(fs);
    auto x = 0.1 * randn(fs);
    // auto y = flt(x);
    flt(inplace(x));

    int p = 0;
}