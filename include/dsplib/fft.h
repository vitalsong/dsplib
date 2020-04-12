#pragma once

#include <dsplib/array.h>

namespace dsplib {

/*!
 * \brief Fast Fourier Transform (complex)
 * \details FFT for complex signal
 * \param arr Input array [N]
 * \return Result array [N]
 */
arr_cmplx fft(const arr_cmplx& arr);

/*!
 * \brief Fast Fourier Transform (real)
 * \details FFT for real signal
 * \param arr Input array [N]
 * \return Result array [N]
 */
arr_cmplx fft(const arr_real& arr);

} ///< dsplib
