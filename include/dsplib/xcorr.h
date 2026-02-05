#pragma once

#include <dsplib/array.h>

namespace dsplib {

/*!
 * \brief Cross correlation
 * \param x1 Firts array [n1]
 * \param x2 Second array [n2]
 * \return Cross correlation result [n1 + n2 - 1]
 */
arr_real xcorr(span_real x1, span_real x2);

/*!
 * \brief Auto correlation
 * \param x Input arry
 * \return Cross correlation result
 */
arr_real xcorr(span_real x);

//cmplx cross-correlation
arr_cmplx xcorr(span_cmplx x1, span_cmplx x2);
arr_cmplx xcorr(span_cmplx x);

} ///< dsplib
