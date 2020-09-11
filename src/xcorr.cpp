#include <dsplib/xcorr.h>
#include <dsplib/fft.h>
#include <dsplib/ifft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

#include <algorithm>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
arr_real xcorr(const arr_real &x1, const arr_real &x2)
{
    const int N1 = x1.size();
    const int N2 = x2.size();
    const int M = 1 << nextpow2(N1 + N2 - 1);

    //padding with zeros
    arr_real y1 = concatenate(x1, arr_real::zeros(M - N1));
    arr_real y2 = concatenate(x2, arr_real::zeros(M - N2));

    //calculation xcorr through FFT/IFFT
    arr_cmplx z1 = conj(fft(y1));
    arr_cmplx z2 = fft(y2);
    arr_real z = real(ifft(z1 * z2));

    //center alignment
    arr_real p1 = z.slice(0, M/2);
    arr_real p2 = z.slice(M/2, M);
    z = concatenate(p2, p1);

    //cut off the excess at the edges (?)
    int m = (M - (N1 + N2 - 1)) / 2;
    return z.slice(m+1, M-m);
}

//-------------------------------------------------------------------------------------------------
arr_real xcorr(const arr_real &x)
{
    return xcorr(x, x);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx xcorr(const arr_cmplx &x1, const arr_cmplx &x2)
{
    const int N1 = x1.size();
    const int N2 = x2.size();
    const int M = 1 << nextpow2(N1 + N2 - 1);

    //padding with zeros
    auto y1 = concatenate(x1, arr_cmplx::zeros(M - N1));
    auto y2 = concatenate(x2, arr_cmplx::zeros(M - N2));

    //calculation xcorr through FFT/IFFT
    auto z1 = conj(fft(y1));
    auto z2 = fft(y2);
    auto z = ifft(z1 * z2);

    //center alignment
    arr_cmplx p1 = z.slice(0, M/2);
    arr_cmplx p2 = z.slice(M/2, M);
    z = concatenate(p2, p1);

    //cut off the excess at the edges (?)
    int m = (M - (N1 + N2 - 1)) / 2;
    return z.slice(m+1, M-m);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx xcorr(const arr_cmplx &x)
{
    return xcorr(x, x);
}

}   ///< dsplib
