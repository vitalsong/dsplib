#pragma once

#include <dsplib/array.h>

namespace dsplib {

//add white Gaussian noise to signal
//in matlab: awgn(x, snr, 'measured')
arr_real awgn(span_real arr, real_t snr);
arr_cmplx awgn(span_cmplx arr, real_t snr);

void awgn(inplace_real arr, real_t snr);
void awgn(inplace_cmplx arr, real_t snr);

}   // namespace dsplib
