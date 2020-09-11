#include <dsplib/hilbert.h>
#include <dsplib/fft.h>
#include <dsplib/ifft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

namespace dsplib {

namespace {
const int DEFAULT_FIR_SIZE = 51;
const real_t DEFAULT_FIR[DEFAULT_FIR_SIZE] = {
    -0.005900976502228,9.074824364018e-16,-0.005928613232103,-2.904033962442e-16,
    -0.008823550945517,-4.908871159007e-17, -0.01259673359506,-9.303461664714e-16,
    -0.01746192064263,1.430779835769e-15, -0.02373102765157,-6.200648271172e-16,
    -0.03188772211027,2.716442846268e-16, -0.04275831095352,-2.21125846066e-17,
    -0.05789285319933,3.157020382665e-16, -0.08065433225117,-5.117701525235e-16,
    -0.1198013399727,1.661661288062e-16,  -0.2076267680754,-1.314802786419e-16,
    -0.6350838033949,                 0,   0.6350838033949,1.314802786419e-16,
    0.2076267680754,-1.661661288062e-16,   0.1198013399727,5.117701525235e-16,
    0.08065433225117,-3.157020382665e-16,  0.05789285319933, 2.21125846066e-17,
    0.04275831095352,-2.716442846268e-16,  0.03188772211027,6.200648271172e-16,
    0.02373102765157,-1.430779835769e-15,  0.01746192064263,9.303461664714e-16,
    0.01259673359506,4.908871159007e-17, 0.008823550945517,2.904033962442e-16,
    0.005928613232103,-9.074824364018e-16, 0.005900976502228
};
}

//-------------------------------------------------------------------------------------------------
hilbert_filter::hilbert_filter() : hilbert_filter(arr_real(DEFAULT_FIR, DEFAULT_FIR_SIZE))
{
    //nothing to do
}

//-------------------------------------------------------------------------------------------------
hilbert_filter::hilbert_filter(const arr_real &h) : _fir(h)
{
    _h = h;
    _d = zeros((h.size()-1)/2);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx hilbert_filter::process(const arr_real &s)
{
    int nd = _d.size();
    int ns = s.size();
    int n = ns + nd;

    //update delay
    //PS: ugly, replace the function delay (_d, s)
    arr_real t = concatenate(_d, s);
    _d = t.slice(n-nd, n);

    //vector for storing the result
    arr_cmplx r(ns);

    //save the I channel
    for (int i=0; i < ns; ++i) {
        r[i].re = t[i];
    }

    //pass the Q channel through the FIR filter
    t = _fir.process(s);

    //save the Q channel
    for (int i=0; i < ns; ++i) {
        r[i].im = t[i];
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
const arr_real &hilbert_filter::impz() const
{
    return _h;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx hilbert(const arr_real &s)
{
    int n = int(1) << nextpow2(s.size());
    arr_real in = concatenate(s, zeros(n - s.size()));

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
