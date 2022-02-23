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

/*!
* \brief Chirp Z-transform
* \param m Transform length
* \param w Ratio between spiral contour points
* \param a Spiral contour initial point
* \return Chirp Z-transform
*/
arr_cmplx czt(arr_cmplx x, real_t m, cmplx_t w = cmplx_t{0.7071, 0.7071}, cmplx_t a = 1);

}   // namespace dsplib
