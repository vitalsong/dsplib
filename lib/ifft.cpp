#include <dsplib/ifft.h>
#include <dsplib/fft.h>
#include <dsplib/math.h>

#include <string.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
arr_cmplx ifft(const arr_cmplx &arr)
{
    int n = arr.size();
    arr_cmplx r = conj(arr);
    r = fft(r);
    r /= n;
    r = conj(r);
    return r;
}

}   ///< dsplib
