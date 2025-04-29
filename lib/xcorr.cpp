#include <dsplib/xcorr.h>
#include <dsplib/fft.h>
#include <dsplib/ifft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

#include <algorithm>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
arr_cmplx xcorr(const arr_cmplx& x1, const arr_cmplx& x2) {
    const int N1 = x1.size();
    const int N2 = x2.size();
    const int M = 1L << nextpow2(N1 + N2 - 1);

    arr_cmplx y1 = x1 | zeros(M - x1.size());
    arr_cmplx y2 = zeros(M - x2.size()) | x2;

    arr_cmplx z1 = conj(fft(y1));
    arr_cmplx z2 = fft(y2);
    arr_cmplx z = conj(ifft(z1 * z2));

    z = flip(z.slice(M - N1 - N2 + 1, M));
    return z;
}

//-------------------------------------------------------------------------------------------------
arr_real xcorr(const arr_real& x1, const arr_real& x2) {
    //TODO: real fft optimization
    return real(xcorr(complex(x1), complex(x2)));
}

//-------------------------------------------------------------------------------------------------
arr_real xcorr(const arr_real& x) {
    return real(xcorr(complex(x), complex(x)));
}

//-------------------------------------------------------------------------------------------------
arr_cmplx xcorr(const arr_cmplx& x) {
    return xcorr(x, x);
}

}   // namespace dsplib
