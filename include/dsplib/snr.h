#pragma once

#include <dsplib/array.h>

namespace dsplib {

/*!
 * \brief Signal to noise and distortion ratio
 * \todo sinad type enum (sig+dist/noise or sig/dist+noise)
 * \param sig time domain signal
 */
real_t sinad(const arr_real& sig);

struct ThdRes
{
    real_t value;        ///< Total harmonic distortion (dB)
    arr_real harmpow;    ///< Power of the harmonics (dB), length depends on nharm (may be -Inf)
    arr_real harmfreq;   ///< Frequencies of the harmonics [0:0.5], length depends on nharm
};

/*!
 * \brief Total harmonic distortion
 * \param sig time domain signal
 * \param nharm number of harmonics
 * \param aliased use harmonics of the fundamental that are aliased into the Nyquist range
 */
ThdRes thd(const arr_real& sig, int nharm = 6, bool aliased = false);

/*!
 * \brief Signal-to-noise ratio
 * \param sig time domain signal
 * \param nharm number of harmonics
 * \param aliased use harmonics of the fundamental that are aliased into the Nyquist range
 */
real_t snr(const arr_real& sig, int nharm = 6, bool aliased = false);

}   // namespace dsplib
