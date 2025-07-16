#include <dsplib/hilbert.h>
#include <dsplib/window.h>
#include <dsplib/fft.h>
#include <dsplib/ifft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

namespace dsplib {

namespace {

arr_real real_hilbert(const arr_cmplx& h) {
    return imag(h) * 2;
}

}   // namespace

HilbertFilter::HilbertFilter(int flen, real_t tw)
  : HilbertFilter(real_hilbert(HilbertFilter::design_fir(flen, 1.0, tw))) {
}

arr_cmplx HilbertFilter::design_fir(int flen, real_t fs, real_t f1) {
    const int M = (flen % 2 == 0) ? (flen + 1) : flen;
    const int N = (int)std::pow(2, nextpow2(8 * M));
    int k1 = (int)std::round(N * f1 / fs);
    k1 = (k1 < 2) ? 2 : k1;       // cannot have dc or fn response
    const int kn = N / 2 + 1;     // bin index at Nyquist limit (1-based)
    const int k2 = kn - k1 + 1;   // high-frequency band edge

    const auto lm = power(arange(k1 - 1) / (k1 - 1), 8);
    const auto rm = flip(lm);
    const arr_cmplx H = complex(concatenate(lm, ones(k2 - k1 + 1), rm, zeros(N / 2 - 1)));
    const arr_cmplx h = ifft(H);   // desired impulse response

    const auto w = window::kaiser(M, 8);
    const auto wzp = concatenate(w.slice(M / 2, M), zeros(N - M), w.slice(0, M / 2));
    const auto hw = wzp * h;   // single-sideband FIR filter, zero-centered
    const auto hh = concatenate(hw.slice(N - (M / 2), N), hw.slice(0, (M + 1) / 2));   // casual FIR
    return hh;
}

HilbertFilter::HilbertFilter(const arr_real& h)
  : _fir(h)
  , _d{h.size() / 2} {
    DSPLIB_ASSERT(firtype(h) == FirType::EvenAntiSym, "Only firtype 3 supported");
}

arr_cmplx HilbertFilter::process(const arr_real& s) {
    const int n = s.size();

    arr_cmplx r(n);

    const auto re = _d.process(s);
    for (int i = 0; i < n; ++i) {
        r[i].re = re[i];
    }

    const auto im = _fir.process(s);
    for (int i = 0; i < n; ++i) {
        r[i].im = im[i];
    }

    return r;
}

const arr_real& HilbertFilter::impz() const {
    return _fir.coeffs();
}

arr_cmplx hilbert(const arr_real& x) {
    const int n = x.size();
    arr_cmplx r = fft(x) * 2;
    r.slice(n / 2 + 1, n) = 0;
    r = ifft(r);
    return r;
}

arr_cmplx hilbert(const arr_real& x, int n) {
    if (n > x.size()) {
        return hilbert(zeropad(x, n));
    }
    if (n < x.size()) {
        return hilbert(x.slice(0, n));
    }
    return hilbert(x);
}

}   // namespace dsplib
