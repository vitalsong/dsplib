#include <dsplib/window.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

#include <cmath>

namespace dsplib {
namespace window {

namespace {

//half window generators

//not exist in matlab, result compatible with scipy/torch
arr_real _cosinewin(int n, int m) noexcept {
    arr_real w(m);
    for (int i = 0; i < m; ++i) {
        w[i] = std::sin(pi / n * (i + 0.5));
    }
    return w;
}

arr_real _hannwin(int n, int m) noexcept {
    arr_real w(m);
    for (int i = 0; i < m; ++i) {
        w[i] = 0.5 - 0.5 * std::cos((2 * pi * i) / (n - 1));
    }
    return w;
}

arr_real _hammingwin(int n, int m) noexcept {
    arr_real w(m);
    for (int i = 0; i < m; ++i) {
        w[i] = 0.54 - 0.46 * std::cos((2 * pi * i) / (n - 1));
    }
    return w;
}

arr_real _blackmanwin(int n, int m) noexcept {
    arr_real w(m);
    for (int i = 0; i < m; ++i) {
        w[i] = 0.42 - 0.5 * std::cos((2 * pi * i) / (n - 1)) + 0.08 * std::cos((4 * pi * i) / (n - 1));
    }
    return w;
}

arr_real _blackmanharriswin(int n, int m) noexcept {
    arr_real w(m);
    const real_t a0 = 0.35875;
    const real_t a1 = 0.48829;
    const real_t a2 = 0.14128;
    const real_t a3 = 0.01168;
    for (int i = 0; i < m; ++i) {
        w[i] = a0 - a1 * std::cos(2 * pi * i / (n - 1)) + a2 * std::cos(4 * pi * i / (n - 1)) -
               a3 * std::cos(6 * pi * i / (n - 1));
    }
    return w;
}

arr_real _gausswin(int n, int m, real_t alpha) {
    const arr_real t = arange(m) - (real_t(n - 1) / 2);
    return exp(-0.5 * abs2(alpha * t / (real_t(n - 1) / 2)));
}

arr_real _tukeywin(int n, int m, real_t ratio) {
    if (ratio <= 0) {
        return ones(m);
    }

    if (ratio >= 1) {
        return _hannwin(n, m);
    }

    arr_real w = ones(m);
    const auto per = ratio / 2;
    const auto tl = std::floor(per * (n - 1)) + 1;
    for (auto i = 0; i < tl; ++i) {
        w[i] = (1 + std::cos(pi / per * (i / real_t(n - 1) - per))) / 2;
    }
    return w;
}

arr_real _sym_window(int n, bool sym, const std::function<arr_real(int n, int m)>& winfn) {
    arr_real w(n);
    const int nl = sym ? 0 : 1;
    const int np = sym ? n : (n + 1);
    if (np % 2 == 0) {
        const int m = np / 2;
        const auto hw = winfn(np, m);
        w.slice(0, m) = hw;
        w.slice(m, n) = flip(hw).slice(0, m - nl);
    } else {
        const int m = (np + 1) / 2;
        const auto hw = winfn(np, m);
        w.slice(0, m) = hw;
        w.slice(m, n) = flip(hw).slice(1, m - nl);
    }
    return w;
}

}   // namespace

arr_real cosine(int n, bool sym) {
    return _sym_window(n, sym, _cosinewin);
}

arr_real hann(int n, bool sym) {
    return _sym_window(n, sym, _hannwin);
}

arr_real hamming(int n, bool sym) {
    return _sym_window(n, sym, _hammingwin);
}

arr_real blackman(int n, bool sym) {
    return _sym_window(n, sym, _blackmanwin);
}

arr_real gauss(int n, real_t alpha, bool sym) {
    return _sym_window(n, sym, [alpha](int n, int m) {
        return _gausswin(n, m, alpha);
    });
}

arr_real blackmanharris(int n, bool sym) {
    return _sym_window(n, sym, _blackmanharriswin);
}

arr_real tukey(int n, real_t r) {
    //TODO: add periodic form?
    return _sym_window(n, true, [r](int n, int m) {
        return _tukeywin(n, m, r);
    });
}

//-------------------------------------------------------------------------------------------------
template<std::size_t N>
static std::array<real_t, N> _init_factorials() {
    std::array<real_t, N> facts;
    static_assert(N > 3);
    facts[0] = 1;
    facts[1] = 1;
    facts[2] = 2;
    for (size_t i = 3; i < N; ++i) {
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
