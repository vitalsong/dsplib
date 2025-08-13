#include "dsplib/resample.h"

#include "dsplib/array.h"
#include "dsplib/fir.h"
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

arr_real _multirate_fir(int interp, int decim, int hlen, real_t beta) {
    const int L = interp;
    const int M = decim;
    const int P = hlen;
    const int R = (L > 1) ? L : M;
    const int N = ((M > L) && (L > 1) && (P * L % M != 0)) ? (2 * P * R + 1) : (2 * P * R);
    const int maxLM = std::max(L, M);
    const auto b = window::kaiser(N + 1, beta);
    //TODO: use lowpasslband(N, maxLM, w)
    auto num = L * fir1(N, (1.0 / maxLM), FilterType::Low, b);
    if (!((L > 1) && (M > L) && (P * L % M != 0))) {
        num = num.slice(0, num.size() - 1);
    }
    return num;
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

std::vector<arr_real> IResampler::polyphase(span_real h, int m, real_t gain, bool flip_coeffs) {
    const int nh = (h.size() % m == 0) ? (h.size()) : ((h.size() / m + 1) * m);
    auto ph = zeropad(h, nh);
    ph /= sum(h);
    assert(nh % m == 0);
    const int n = nh / m;
    auto r = std::vector<arr_real>(m, zeros(n));
    for (int i = 0; i < m; ++i) {
        int ih = (m + i) % m;
        for (int k = 0; k < n; ++k) {
            r[i][k] = ph[ih] * gain;
            ih += m;
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
    const int mdl = dl * q / p;
    const int nn = IResampler::next_size(nx + mdl, p, q);
    const auto xx = zeropad(x, nn);
    const auto y = *rsmp.process(xx).slice(dl, dl + ny);
    return y;
}

}   // namespace dsplib