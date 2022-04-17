#include <dsplib/gccphat.h>

namespace dsplib {

real_t gccphat(const arr_real& sig, const arr_real& refsig, int fs) {
    const real_t ts = 1.0 / fs;
    auto X1 = fft(sig);
    auto X2 = conj(fft(refsig));
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
    return delay;
}

arr_real gccphat(const std::vector<arr_real>& sig, const arr_real& refsig, int fs) {
    const real_t ts = 1.0 / fs;
    const auto X2 = conj(fft(refsig));
    const int M = X2.size();
    const int M2 = M / 2;
    arr_real delays = zeros(sig.size());
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
        delays[i] = delay;
    }
    return delays;
}

}   // namespace dsplib