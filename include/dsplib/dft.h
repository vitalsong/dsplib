#pragma once

#include <dsplib/array.h>

namespace dsplib {

/*!
 * \brief Discrete Fourier Transform (complex)
 * \details DFT for complex signal (N != 2^M)
 * \param arr Input array [N]
 * \return Result array [N]
 */
arr_cmplx dft(const arr_cmplx& arr);

/*!
 * \brief Discrete Fourier Transform (real)
 * \details DFT for real signal (N != 2^M)
 * \param arr Input array [N]
 * \return Result array [N]
 */
arr_cmplx dft(const arr_real& arr);

/*!
 * \brief Inverse discrete Fourier Transform
 * \details IDFT for complex signal (N != 2^M)
 * \param arr Input array [N]
 * \return Result array [N]
 */
arr_cmplx idft(const arr_cmplx& arr);

} ///< dsplib
