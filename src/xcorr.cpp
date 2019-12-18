#include <dsplib/xcorr.h>
#include <dsplib/fft.h>
#include <dsplib/ifft.h>
#include <dsplib/math.h>

#include <algorithm>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
arr_real xcorr(const arr_real &x1, const arr_real &x2)
{
    const int N1 = x1.size();
    const int N2 = x2.size();
    const int M = 1 << nextpow2(N1 + N2 - 1);

    //padding with zeros
    arr_real y1 = arr_real::join(x1, arr_real::zeros(M - N1));
    arr_real y2 = arr_real::join(x2, arr_real::zeros(M - N2));

    //calculation xcorr through FFT/IFFT
    arr_cmplx z1 = conj(fft(y1));
    arr_cmplx z2 = fft(y2);
    arr_real z = real(ifft(z1 * z2));

    //center alignment
    arr_real p1 = arr_real::slice(z, 0, M/2-1);
    arr_real p2 = arr_real::slice(z, M/2, M-1);
    z = arr_real::join(p2, p1);

    //cut off the excess at the edges (?)
    int m = (M - (N1 + N2 - 1)) / 2;
    arr_real r = arr_real::slice(z, m+1, M-m-1);

    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real xcorr(const arr_real &x)
{
    return xcorr(x, x);
}

}   ///< dsplib
