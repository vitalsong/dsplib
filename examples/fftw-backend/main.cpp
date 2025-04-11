#include <dsplib.h>

int main() {
    using namespace std::complex_literals;
    {
        dsplib::arr_real x = {1, 2, 3, 4};
        auto y = dsplib::fft(x);
        dsplib::arr_cmplx d = {10.0000 + 0.0000i, -2.0000 + 2.0000i, -2.0000 + 0.0000i, -2.0000 - 2.0000i};
        auto e = dsplib::max(dsplib::abs(y - d));
        DSPLIB_ASSERT(e < dsplib::eps(), "transform error");

        auto ix = dsplib::irfft(y);
        auto ie = dsplib::max(dsplib::abs(ix - x));
        DSPLIB_ASSERT(ie < dsplib::eps(), "transform error");
    }

    {
        dsplib::arr_cmplx x = {1 + 1i, 2 + 2i, 3 + 3i, 4 + 4i};
        auto y = dsplib::fft(x);
        dsplib::arr_cmplx d = {10.0000 + 10.0000i, -4.0000 + 0.0000i, -2.0000 - 2.0000i, 0.0000 - 4.0000i};
        auto e = dsplib::max(dsplib::abs(y - d));
        DSPLIB_ASSERT(e < dsplib::eps(), "transform error");

        auto ix = dsplib::ifft(y);
        auto ie = dsplib::max(dsplib::abs(ix - x));
        DSPLIB_ASSERT(ie < dsplib::eps(), "transform error");
    }

    return 0;
}