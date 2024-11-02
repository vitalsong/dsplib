#include "tests_common.h"

#include <cstdlib>
#include <dsplib.h>

TEST(ArrApply, Real) {
    dsplib::arr_real x = {0, -1, -2, -3, -4, -5};
    {
        auto y = x.apply(+[](const dsplib::real_t& v) {
            return std::abs(v);
        });
        ASSERT_EQ_ARR_REAL(y, dsplib::abs(x));
    }
    {
        int s = 10;
        auto y = x.apply([s](const dsplib::real_t& v) {
            return std::abs(v) + s;
        });
        ASSERT_EQ_ARR_REAL(y, dsplib::abs(x) + s);
    }
}

TEST(ArrApply, Cmplx) {
    using namespace std::complex_literals;
    dsplib::arr_cmplx x = {0 + 1i, -1 + 2i, -2 + 3i, -3 + 4i, -4 + 5i, -5 + 6i};
    {
        auto y = x.apply(+[](const dsplib::cmplx_t& v) {
            return std::sqrt(v.re * v.re + v.im * v.im);
        });
        ASSERT_EQ_ARR_REAL(y, dsplib::abs(x));
    }
    {
        int s = 10;
        auto y = x.apply([s](const dsplib::cmplx_t& v) {
            return std::sqrt(v.re * v.re + v.im * v.im) + s;
        });
        ASSERT_EQ_ARR_REAL(y, dsplib::abs(x) + s);
    }
    {
        auto y = x.apply(+[](const dsplib::cmplx_t& v) {
            return v + 2;
        });
        ASSERT_EQ_ARR_CMPLX(y, x + 2);
    }
}
