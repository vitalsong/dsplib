#pragma once

#include <dsplib/array.h>

namespace dsplib {

//add white Gaussian noise to signal
arr_real awgn(const arr_real& arr, real_t snr);
arr_cmplx awgn(const arr_cmplx& arr, real_t snr);

}   ///< dsplib
