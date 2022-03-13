#include <dsplib/fft.h>
#include <dsplib/math.h>
#include <dsplib/czt.h>

#include <string.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
arr_cmplx dft(const arr_cmplx& x) {
    const int n = x.size();
    const cmplx_t w = expj(-2 * pi / n);
    return czt(x, n, w);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx idft(const arr_cmplx& x) {
    const int n = x.size();
    return conj(dft(conj(x)) / n);
}

}   // namespace dsplib
