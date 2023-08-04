#include "dsplib/resample.h"

#include "dsplib/array.h"
#include "dsplib/fir.h"
#include "dsplib/window.h"

#include <numeric>

namespace dsplib {

namespace {

class BypassResampler : public IResampler
{
public:
    explicit BypassResampler() = default;
    arr_real process(const arr_real& sig) final {
        return sig;
    }
};

}   // namespace

//------------------------------------------------------------------------------
arr_real design_multirate_fir(int interp, int decim, int hlen, real_t astop) {
    const int L = interp;
    const int M = decim;
    const int P = hlen;
    const int R = (L > 1) ? L : M;
    const int N = ((M > L) && (L > 1) && (P * L % M != 0)) ? (2 * P * R + 1) : (2 * P * R);
    real_t beta = 0;
    if (astop >= 50) {
        beta = 0.1102 * (astop - 8.71);
    } else if ((astop < 50) && (astop > 21)) {
        beta = 0.5842 * std::pow(astop - 21, 0.4) + 0.07886 * (astop - 21);
    }
    const int maxLM = std::max(L, M);
    const auto b = window::kaiser(N + 1, beta);
    //TODO: use lowpasslband(N, maxLM, w)
    auto num = L * fir1(N, (1.0 / maxLM), FilterType::Low, b);
    if (!((L > 1) && (M > L) && (P * L % M != 0))) {
        num = num.slice(0, num.size() - 1);
    }
    return num;
}

std::vector<arr_real> IResampler::polyphase(arr_real h, int m, real_t gain) {
    const int nh = (h.size() % m == 0) ? (h.size()) : ((h.size() / m + 1) * m);
    h = zeropad(h, nh);
    h /= sum(h);
    assert(nh % m == 0);
    const int n = nh / m;
    auto r = std::vector<arr_real>(m, zeros(n));
    for (int i = 0; i < m; ++i) {
        int ih = (m + i) % m;
        for (int k = 0; k < n; ++k) {
            r[i][k] = h[ih] * gain;
            ih += m;
        }
    }
    return r;
}

int IResampler::next_size(int size, int p, int q) {
    int gcd = std::gcd(p, q);
    int d = q / gcd;
    size = (size % d == 0) ? (size) : ((size / d + 1) * d);
    return size;
}

int IResampler::prev_size(int size, int p, int q) {
    int gcd = std::gcd(p, q);
    int d = q / gcd;
    size = (size % d == 0) ? (size) : (size / d * d);
    return size;
}

//------------------------------------------------------------------------------
FIRResampler::FIRResampler(int out_fs, int in_fs) {
    int m = 1;
    int d = 1;

    if (in_fs == out_fs) {
        rsmp_ = std::make_shared<BypassResampler>();
        mode_ = Mode::Bypass;
    } else {
        auto gcd = std::gcd(in_fs, out_fs);
        m = out_fs / gcd;
        d = in_fs / gcd;
    }

    const auto h = design_multirate_fir(m, d);

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

arr_real FIRResampler::process(const arr_real& sig) {
    return rsmp_->process(sig);
}

//------------------------------------------------------------------------------
arr_real resample(const arr_real& x, int p, int q) {
    FIRResampler rsmp(p, q);
    p = rsmp.interp_rate();
    q = rsmp.decim_rate();
    const int nr = x.size() * p / q;
    const int dl = rsmp.delay();
    const int nf = IResampler::next_size(dl * q / p, p, q);
    auto out = rsmp.process(x) | rsmp.process(zeros(nf));
    out = out.slice(dl, dl + nr);
    return out;
}

}   // namespace dsplib