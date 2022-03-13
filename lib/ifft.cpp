#include <dsplib/ifft.h>
#include <dsplib/fft.h>
#include <dsplib/math.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
arr_cmplx ifft(const arr_cmplx& x) {
    const int n = x.size();
    arr_cmplx r = conj(x);
    r = fft(r);
    r /= n;
    r = conj(r);
    return r;
}

}   // namespace dsplib
