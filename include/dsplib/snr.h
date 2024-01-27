#pragma once

#include <dsplib/array.h>

namespace dsplib {

enum class SinadType
{
    Time,   ///< calculate periodogram for time domain signal x
    Psd,    ///< represent input signal x as precalculated PSD estimate
    Power   ///< represent input signal x as precalculated spectrum power
};

/*!
 * \brief Signal to noise and distortion ratio
 * \todo sinad type enum (sig+dist/noise or sig/dist+noise)
 * \param sig input signal
 * \param type input signal type
 */
real_t sinad(const arr_real& sig, SinadType type = SinadType::Time);

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
ThdRes thd(const arr_real& sig, int nharm = 6, bool aliased = false, SinadType type = SinadType::Time);

/*!
 * \brief Signal-to-noise ratio
 * \param sig time domain signal
 * \param nharm number of harmonics
 * \param aliased use harmonics of the fundamental that are aliased into the Nyquist range
 */
real_t snr(const arr_real& sig, int nharm = 6, bool aliased = false, SinadType type = SinadType::Time);

}   // namespace dsplib
