#ifndef AWGN_H
#define AWGN_H

#include <dsplib/array.h>

namespace dsplib {

arr_real awgn(const arr_real& arr, real_t snr);
arr_cmplx awgn(const arr_cmplx& arr, real_t snr);

}   ///< dsplib

#endif // AWGN_H
