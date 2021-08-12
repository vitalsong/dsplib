#pragma once

#include <dsplib/array.h>

namespace dsplib {

/*!
 * \brief Signal to noise and distortion ratio
 */
real_t sinad(const arr_real& x);

}   // namespace dsplib
