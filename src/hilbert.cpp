#include <dsplib/hilbert.h>
#include <dsplib/fft.h>
#include <dsplib/ifft.h>
#include <dsplib/math.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
hilbert::hilbert(const arr_real &h) : _fir(h)
{
    _h = h;
    _d = arr_real((h.size()-1)/2);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx hilbert::process(const arr_real &s)
{
    int nd = _d.size();
    int ns = s.size();
    int n = ns + nd;

    //update delay
    //PS: ugly, replace the function delay (_d, s)
    arr_real t = arr_real::join(_d, s);
    _d = arr_real::slice(t, n-nd, n-1);

    //vector for storing the result
    arr_cmplx r(ns);

    //save the I channel
    for (int i=0; i < ns; ++i) {
        r[i].xi = t[i];
    }

    //pass the Q channel through the FIR filter
    t = _fir.process(s);

    //save the Q channel
    for (int i=0; i < ns; ++i) {
        r[i].xq = t[i];
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
const arr_real &hilbert::impz() const
{
    return _h;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx hilbert::dftcalc(const arr_real &s)
{
    int n = int(1) << nextpow2(s.size());
    arr_real in = arr_real::join(s, arr_real::zeros(n - s.size()));

    //direct DFT
    arr_cmplx r = fft(in);

    //zeroing the second half
    for (int i=n/2+1; i < n; ++i) {
        r[i] = 0;
    }

    //coefficient calibration
    for (int i=1; i < n-1; ++i) {
        r[i] *= 2;
    }

    //inverse DFT
    r = ifft(r);

    return r;
}

}   ///< dsplib
