#include <dsplib/awgn.h>
#include <dsplib/math.h>
#include <dsplib/random.h>

#include <cstdlib>
#include <cmath>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
arr_real awgn(const arr_real& arr, real_t snr) {
    real_t stddev = rms(arr) * std::pow(10, ((-1) * snr / 20));
    arr_real r(arr);
    r += randn(r.size()) * stddev;
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx awgn(const arr_cmplx& arr, real_t snr) {
    real_t stddev = 0.5 * rms(arr) * std::pow(10, ((-1) * snr / 20));
    arr_cmplx r(arr);
    r += complex(randn(r.size()) * stddev, randn(r.size()) * stddev);
    return r;
}

}   // namespace dsplib
