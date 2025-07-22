#include <dsplib/gccphat.h>

namespace dsplib {

gccphat_res_t gccphat(const arr_real& sig, const arr_real& refsig, int fs) {
    const real_t ts = 1.0 / fs;
    auto X1 = fft(sig);
    auto X2 = fft(refsig);
    conj(inplace(X2));
    auto Y = X1 * X2;
    const auto R = ifft(Y / abs(Y));
    const auto n = argmax(R);
    const int M = R.size();
    const int M2 = R.size() / 2;
    auto peak = peakloc(R, n);
    real_t delay = 0;
    if (peak < M2) {
        delay = peak * ts;
    } else {
        delay = (peak - M) * ts;
    }

    gccphat_res_t res;
    res.tau = delay;
    res.corr = R;
    return res;
}

gccphat_res_ch_t gccphat(const std::vector<arr_real>& sig, const arr_real& refsig, int fs) {
    const real_t ts = 1.0 / fs;
    auto X2 = fft(refsig);
    conj(inplace(X2));
    const int M = X2.size();
    const int M2 = M / 2;
    gccphat_res_ch_t res;
    res.tau = zeros(sig.size());
    res.corr.resize(sig.size());
    for (size_t i = 0; i < sig.size(); i++) {
        auto X1 = fft(sig[i]);
        auto Y = X1 * X2;
        const auto R = ifft(Y / abs(Y));
        const auto n = argmax(R);
        auto peak = peakloc(R, n);
        real_t delay = 0;
        if (peak < M2) {
            delay = peak * ts;
        } else {
            delay = (peak - M) * ts;
        }
        res.tau[i] = delay;
        res.corr[i] = R;
    }
    return res;
}

}   // namespace dsplib