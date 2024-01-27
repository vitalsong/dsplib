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
    const arr_real t = arange(n) - (m / 2);
    return exp(-0.5 * abs2(alpha * t / (m / 2)));
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

//-------------------------------------------------------------------------------------------------
template<std::size_t N>
static std::array<real_t, N> _init_factorials() {
    std::array<real_t, N> facts;
    static_assert(N > 3);
    facts[0] = 1;
    facts[1] = 1;
    facts[2] = 2;
    for (int i = 3; i < N; ++i) {
        facts[i] = facts[i - 1] * i;
    }
    return facts;
}

// zero-order modified Bessel function of the first kind
static real_t _besseli0(real_t x) {
    constexpr int num_steps = 15;
    static const auto factorials = _init_factorials<num_steps>();
    real_t r = 0;
    for (int k = 0; k < num_steps; ++k) {
        r += std::pow(std::pow(x / 2, k) / factorials[k], 2);
    }
    return r;
}

arr_real kaiser(int nw, real_t beta) {
    const real_t bes = abs(_besseli0(beta));
    const int odd = nw % 2;
    const auto xind = abs2(nw - 1);
    const int n = (nw + 1) / 2;
    auto w = zeros(n);
    for (int i = 0; i < n; ++i) {
        const auto xi = 4 * abs2(i + 0.5 * (1 - odd));
        w[i] = std::abs(_besseli0(beta * std::sqrt(1 - (xi / xind))) / bes);
    }
    const arr_real wl = flip(*w.slice(odd, n));
    return (wl | w);
}

}   // namespace window
}   // namespace dsplib
