#include "dsplib/assert.h"
#include "dsplib/spectrum.h"
#include "dsplib/array.h"
#include "dsplib/fft.h"
#include "dsplib/math.h"
#include "dsplib/window.h"

namespace dsplib {

namespace {

arr_real _mscohere(const arr_real& x, const arr_real& y, const arr_real& win, int noverlap, int nfft) {
    const int N = x.size();
    const int winlen = win.size();
    DSPLIB_ASSERT(x.size() == y.size(), "input size error");
    DSPLIB_ASSERT(ispow2(nfft), "fft size must be power of 2");
    DSPLIB_ASSERT(noverlap < winlen, "noverlap must be less than winlen");
    const int stride = winlen - noverlap;
    const int num_segments = (N - winlen) / stride + 1;

    arr_real Pxx(nfft / 2 + 1);
    arr_real Pyy(nfft / 2 + 1);
    arr_cmplx Pxy(nfft / 2 + 1);
    arr_cmplx X(nfft / 2 + 1);
    arr_cmplx Y(nfft / 2 + 1);
    arr_real px(winlen);
    arr_real py(winlen);

    for (int i = 0; i < num_segments; ++i) {
        int t1 = (i * stride);
        int t2 = t1 + winlen;

        px.slice(0, winlen) = x.slice(t1, t2);
        px *= win;
        X = fft(px, nfft).slice(0, nfft / 2 + 1);
        Pxx += abs2(X);

        py.slice(0, winlen) = y.slice(t1, t2);
        py *= win;
        Y = fft(py, nfft).slice(0, nfft / 2 + 1);
        Pyy += abs2(Y);

        Pxy += X * conj(Y);
    }

    return abs2(Pxy) / (Pxx * Pyy);
}

int _nextfft(int n) noexcept {
    return 1L << nextpow2(n);
}

}   // namespace

arr_real mscohere(const arr_real& x, const arr_real& y, int winlen) {
    return mscohere(x, y, window::hamming(winlen), (winlen / 2), _nextfft(winlen));
}

arr_real mscohere(const arr_real& x, const arr_real& y, const arr_real& win) {
    const int winlen = win.size();
    return mscohere(x, y, win, (winlen / 2), _nextfft(winlen));
}

arr_real mscohere(const arr_real& x, const arr_real& y, int winlen, int noverlap, int nfft) {
    return mscohere(x, y, window::hamming(winlen), noverlap, nfft);
}

arr_real mscohere(const arr_real& x, const arr_real& y, const arr_real& win, int noverlap, int nfft) {
    return _mscohere(x, y, win, noverlap, nfft);
}

}   // namespace dsplib