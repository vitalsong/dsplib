#pragma once

#include <dsplib/types.h>
#include <dsplib/array.h>

namespace dsplib {

/*!
 * \brief Inverse fourier transform
 * \details IFFT for complex signal
 * \param arr Input array [N]
 * \return Result array [N]
 */
arr_cmplx ifft(const arr_cmplx& arr);

} ///< dsplib
