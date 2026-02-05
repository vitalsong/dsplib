#include <dsplib/xcorr.h>
#include <dsplib/fft.h>
#include <dsplib/ifft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

namespace dsplib {

namespace {

template<typename T>
arr_cmplx _xcorr(span_t<T> x1, span_t<T> x2) {
    const int N1 = x1.size();
    const int N2 = x2.size();
    const int M = 1L << nextpow2(N1 + N2 - 1);

    base_array<T> y1(M);
    y1.slice(0, x1.size()).assign(x1);

    base_array<T> y2(M);
    y2.slice(M - x2.size(), M).assign(x2);

    auto z1 = fft(y1);
    conj(inplace(z1));
    auto z2 = fft(y2);
    z1 *= z2;
    auto z = ifft(z1);
    conj(inplace(z));

    z = flip(z.slice(M - N1 - N2 + 1, M));
    return z;
}

}   // namespace

arr_cmplx xcorr(span_cmplx x1, span_cmplx x2) {
    return _xcorr(x1, x2);
}

arr_real xcorr(span_real x1, span_real x2) {
    return real(_xcorr(x1, x2));
}

arr_real xcorr(span_real x) {
    return real(_xcorr(x, x));
}

arr_cmplx xcorr(span_cmplx x) {
    return _xcorr(x, x);
}

}   // namespace dsplib
