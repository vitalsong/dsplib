#include "dsplib/resample.h"

#include "dsplib/array.h"
#include "dsplib/window.h"
#include "dsplib/utils.h"

#include <numeric>
#include <cassert>

namespace dsplib {

namespace {

class BypassResampler : public IResampler
{
public:
    explicit BypassResampler() = default;
    arr_real process(span_real sig) final {
        return sig;
    }
};

//TODO: add to math module
void _sinc(dsplib::inplace_real x) {
    auto px = x.get();
    const int n = px.size();
    for (int i = 0; i < n; ++i) {
        if (px[i] != 0) {
            px[i] = std::sin(dsplib::pi * px[i]) / (dsplib::pi * px[i]);
        } else {
            px[i] = 1.0;
        }
    }
}

dsplib::arr_real _lowpasslband(int N, int L, dsplib::span_real win) {
    DSPLIB_ASSUME(N % 2 == 0);

    const dsplib::real_t M = N / 2;
    dsplib::arr_real b = dsplib::arange(-M, M + 1);
    b /= L;
    _sinc(dsplib::inplace(b));
    b *= (1.0 / L);
    b *= win;

    // force set sinc zeros
    //TODO: use slices
    for (int i = M + L; i < b.size(); i += L) {
        b[i] = 0;
    }
    for (int i = M - L; i >= 0; i -= L) {
        b[i] = 0;
    }

    return b;
}

arr_real _multirate_fir(int interp, int decim, int hlen, real_t beta) {
    const int L = interp;
    const int M = decim;
    const int P = hlen;
    const int R = (L > 1) ? L : M;
    const int N = 2 * P * R;
    const int band = std::max(L, M);
    const auto win = window::kaiser(N + 1, beta);
    auto h = _lowpasslband(N, band, win);
    return h;
}

}   // namespace

//------------------------------------------------------------------------------
arr_real design_multirate_fir(int interp, int decim, int hlen, real_t astop) {
    auto [p, q] = IResampler::simplify(interp, decim);
    if (p == q) {
        return {1.0};
    }

    real_t beta = 5.0;
    if (astop >= 50) {
        beta = 0.1102 * (astop - 8.71);
    } else if ((astop < 50) && (astop > 21)) {
        beta = 0.5842 * std::pow(astop - 21, 0.4) + 0.07886 * (astop - 21);
    }
    return _multirate_fir(p, q, hlen, beta);
}

//TODO: remove `flip_coeffs`?
std::vector<arr_real> IResampler::polyphase(span_real h, int m, real_t gain, bool flip_coeffs) {
    const int nh = int(h.size());
    const int n = (nh + m - 1) / m;   // ceil(Nh/m)

    auto r = std::vector<arr_real>(m, zeros(n));
    for (int i = 0; i < m; ++i) {
        for (int k = 0; k < n; ++k) {
            const int ih = i + k * m;
            r[i][k] = (ih < nh) ? (h[ih] * gain) : real_t(0);
        }
    }

    if (flip_coeffs) {
        for (int i = 0; i < m; ++i) {
            flip(inplace(r[i]));
        }
    }

    return r;
}

std::pair<int, int> IResampler::simplify(int p, int q) {
    int gcd = std::gcd(p, q);
    p /= gcd;
    q /= gcd;
    return std::make_pair(p, q);
}

int IResampler::next_size(int size, int p, int q) {
    auto [_, d] = simplify(p, q);
    size = (size % d == 0) ? (size) : ((size / d + 1) * d);
    return size;
}

int IResampler::prev_size(int size, int p, int q) {
    auto [_, d] = simplify(p, q);
    size = (size % d == 0) ? (size) : (size / d * d);
    return size;
}

//------------------------------------------------------------------------------
FIRResampler::FIRResampler(int out_fs, int in_fs)
  : FIRResampler(out_fs, in_fs, design_multirate_fir(out_fs, in_fs)) {
}

FIRResampler::FIRResampler(int out_fs, int in_fs, span_real h) {
    const auto [m, d] = IResampler::simplify(out_fs, in_fs);

    if (m == d) {
        rsmp_ = std::make_shared<BypassResampler>();
        mode_ = Mode::Bypass;
        return;
    }

    if ((d > 1) && (m == 1)) {
        rsmp_ = std::make_shared<FIRDecimator>(d, h);
        mode_ = Mode::Decimator;
    }

    if ((d == 1) && (m > 1)) {
        rsmp_ = std::make_shared<FIRInterpolator>(m, h);
        mode_ = Mode::Interpolator;
    }

    if ((d > 1) && (m > 1)) {
        rsmp_ = std::make_shared<FIRRateConverter>(m, d, h);
        mode_ = Mode::Resampler;
    }
}

int FIRResampler::delay() const noexcept {
    return rsmp_->delay();
}

int FIRResampler::interp_rate() const noexcept {
    return rsmp_->interp_rate();
}

int FIRResampler::decim_rate() const noexcept {
    return rsmp_->decim_rate();
}

arr_real FIRResampler::process(span_real sig) {
    return rsmp_->process(sig);
}

//------------------------------------------------------------------------------
arr_real resample(span_real x, int p_, int q_, int n, real_t beta) {
    const auto [p, q] = IResampler::simplify(p_, q_);
    if (p == q) {
        return x;
    }

    const auto h = _multirate_fir(p, q, n, beta);
    return resample(x, p, q, h);
}

arr_real resample(span_real x, int p_, int q_, span_real h) {
    const auto [p, q] = IResampler::simplify(p_, q_);
    if (p == q) {
        return x;
    }

    FIRResampler rsmp(p, q, h);
    const int nx = IResampler::next_size(x.size(), p, q);
    const int ny = nx * p / q;
    const int dl = rsmp.delay();
    const int mdl = dl * q / p + 1;
    const int nn = IResampler::next_size(nx + mdl, p, q);
    const auto xx = zeropad(x, nn);
    auto y = rsmp.process(xx);
    return y.slice(dl, dl + ny).copy();
}

}   // namespace dsplib