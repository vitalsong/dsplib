#include <dsplib/window.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

#include <cmath>

namespace dsplib {
namespace window {

//-------------------------------------------------------------------------------------------------
arr_real rectangle(int n) {
    arr_real arr(n);
    for (int i = 0; i < n; ++i) {
        arr[i] = 1;
    }
    return arr;
}

//-------------------------------------------------------------------------------------------------
arr_real sinus(int n) {
    arr_real arr(n);
    for (int i = 0; i < n; ++i) {
        arr[i] = std::sin((pi * i) / (n - 1));
    }
    return arr;
}

//-------------------------------------------------------------------------------------------------
arr_real hann(int n) {
    arr_real arr(n);
    for (int i = 0; i < n; ++i) {
        arr[i] = 0.5 - 0.5 * std::cos((2 * pi * i) / (n - 1));
    }
    return arr;
}

//-------------------------------------------------------------------------------------------------
arr_real hamming(int n) {
    arr_real arr(n);
    for (int i = 0; i < n; ++i) {
        arr[i] = 0.54 - 0.46 * std::cos((2 * pi * i) / (n - 1));
    }
    return arr;
}

//-------------------------------------------------------------------------------------------------
arr_real blackman(int n) {
    arr_real arr(n);
    for (int i = 0; i < n; ++i) {
        arr[i] = 0.42 - 0.5 * std::cos((2 * pi * i) / (n - 1)) + 0.08 * std::cos((4 * pi * i) / (n - 1));
    }
    return arr;
}

//-------------------------------------------------------------------------------------------------
arr_real gauss(int n, real_t alpha) {
    const real_t m = n - 1;
    arr_real t = range(n) - (m / 2);
    auto r = exp(-0.5 * pow2(alpha * t / (m / 2)));
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real blackmanharris(int n, bool periodic) {
    const real_t a0 = 0.35875;
    const real_t a1 = 0.48829;
    const real_t a2 = 0.14128;
    const real_t a3 = 0.01168;
    const int den = (periodic) ? (n) : (n - 1);
    arr_real r(n);
    for (int i = 0; i < n; ++i) {
        r[i] = a0 - a1 * std::cos(2 * pi * i / den) + a2 * std::cos(4 * pi * i / den) - a3 * std::cos(6 * pi * i / den);
    }
    return r;
}

}   // namespace window
}   // namespace dsplib
