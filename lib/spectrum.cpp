#include "dsplib/spectrum.h"
#include "dsplib/array.h"
#include "dsplib/fft.h"
#include "dsplib/keywords.h"
#include "dsplib/math.h"
#include "dsplib/throw.h"
#include "dsplib/types.h"
#include "dsplib/utils.h"
#include "dsplib/window.h"

namespace dsplib {

namespace {

template<typename T>
arr_real _calcspec(const base_array<T>& x, const arr_real& win, int noverlap, int nfft, SpectrumType type) {
    const int N = x.size();
    const int winlen = win.size();
    DSPLIB_ASSERT(ispow2(nfft), "fft size must be power of 2");
    DSPLIB_ASSERT(noverlap < winlen, "noverlap must be less than winlen");
    const int stride = winlen - noverlap;
    const int num_segments = (N - winlen) / stride + 1;

    //compensates window power
    const auto winpow = (type == SpectrumType::Psd) ? dot(win, win) : pow2(sum(win));

    arr_real pxx(nfft);
    base_array<T> seg(winlen);
    for (int i = 0; i < num_segments; ++i) {
        int t1 = (i * stride);
        int t2 = t1 + winlen;
        seg.slice(0, winlen) = x.slice(t1, t2);
        seg *= win;
        pxx += dsplib::abs2(dsplib::fft(seg, nfft)) / winpow;
    }
    pxx /= num_segments;
    return pxx;
}

WelchResult _welch(const arr_real& x, const arr_real& win, int noverlap, int nfft, SpectrumType type) {
    auto pxx = _calcspec(x, win, noverlap, nfft, type);
    pxx = pxx.slice(0, nfft / 2 + 1);
    pxx[0] /= 2;
    pxx[-1] /= 2;
    pxx *= 2;
    const auto f = arange(0, nfft / 2 + 1) / nfft;
    return WelchResult{pxx, f};
}

WelchResult _welch(const arr_cmplx& x, const arr_real& win, int noverlap, int nfft, SpectrumType type) {
    auto pxx = _calcspec(x, win, noverlap, nfft, type);
    const auto f = arange(-nfft / 2 + 1, nfft / 2 + 1) / nfft;
    return WelchResult{pxx, f};
}

}   // namespace

//----------------------------------------------------------------------------
WelchResult welch(const arr_real& x, const arr_real& win, int noverlap, int nfft, SpectrumType type) {
    return _welch(x, win, noverlap, nfft, type);
}

WelchResult welch(const arr_real& x, int winlen, int noverlap, int nfft, SpectrumType type) {
    return welch(x, window::hamming(winlen), noverlap, nfft, type);
}

WelchResult welch(const arr_real& x, int winlen, SpectrumType type) {
    return welch(x, window::hamming(winlen), type);
}

WelchResult welch(const arr_real& x, const arr_real& win, SpectrumType type) {
    const int nfft = int(1) << nextpow2(win.size());
    return welch(x, win, (win.size() / 2), nfft, type);
}

//----------------------------------------------------------------------------
WelchResult welch(const arr_cmplx& x, const arr_real& win, int noverlap, int nfft, SpectrumType type) {
    return _welch(x, win, noverlap, nfft, type);
}

WelchResult welch(const arr_cmplx& x, int winlen, int noverlap, int nfft, SpectrumType type) {
    return welch(x, window::hamming(winlen), noverlap, nfft, type);
}

WelchResult welch(const arr_cmplx& x, int winlen, SpectrumType type) {
    return welch(x, window::hamming(winlen), type);
}

WelchResult welch(const arr_cmplx& x, const arr_real& win, SpectrumType type) {
    const int nfft = int(1) << nextpow2(win.size());
    return welch(x, win, (win.size() / 2), nfft, type);
}

}   // namespace dsplib