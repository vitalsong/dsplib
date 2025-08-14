#include <dsplib/awgn.h>
#include <dsplib/math.h>
#include <dsplib/random.h>

#include <cstdlib>
#include <cmath>

namespace dsplib {

arr_real awgn(span_real arr, real_t snr) {
    arr_real r(arr);
    awgn(inplace(r), snr);
    return r;
}

arr_cmplx awgn(span_cmplx arr, real_t snr) {
    arr_cmplx r(arr);
    awgn(inplace(r), snr);
    return r;
}

void awgn(inplace_real arr, real_t snr) {
    auto x = arr.get();
    const int n = x.size();
    const real_t stddev = rms(x) * std::pow(10, ((-1) * snr / 20));
    const auto noise = randn(n);
    for (int i = 0; i < n; ++i) {
        x[i] += noise[i] * stddev;
    }
}

void awgn(inplace_cmplx arr, real_t snr) {
    auto x = arr.get();
    const int n = x.size();
    const real_t stddev = 0.5 * rms(x) * std::pow(10, ((-1) * snr / 20));
    const auto noise = randn(2 * n);
    for (int i = 0; i < n; ++i) {
        x[i].re += noise[2 * i] * stddev;
        x[i].im += noise[2 * i + 1] * stddev;
    }
}

}   // namespace dsplib
