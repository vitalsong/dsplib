#pragma once

#include <dsplib/array.h>

namespace dsplib {

enum class StftRange
{
    Centered,
    Twosided,
    Onesided
};

enum class OverlapMethod
{
    Ola,    ///< Overlap-Add
    Wola,   ///< Weighted Overlap-Add
};

//determine whether window-overlap combination is COLA compliant
bool iscola(const arr_real& win, int noverlap, OverlapMethod method = OverlapMethod::Ola);

/*!
 * \brief Short Time Fourier Transform
 * \todo return time and freq vector
 * \todo complex input support
 * \warning Only real output signal support (conjugate symmetry by default)
 * \param x input signal
 * \param win windowing function
 * \param overlap overlap length
 * \param nfft number of DFT points
 * \param range STFT frequency range
 * \return short time fourier transform
 */
std::vector<arr_cmplx> stft(const arr_real& x, const arr_real& win, int overlap, int nfft,
                            StftRange range = StftRange::Onesided);

//win: hann(nfft, `periodic`)
//overlap: nfft/2
std::vector<arr_cmplx> stft(const arr_real& x, int nfft, StftRange range = StftRange::Onesided);

/*!
 * \brief Inverse Short Time Fourier Transform
 * \warning Only real output signal support (conjugate symmetry by default)
 * \param x short time fourier transform
 * \param win windowing function
 * \param overlap overlap length
 * \param nfft number of DFT points
 * \param range STFT frequency range
 * \param method method of overlap-add (ola/wola)
 * \return reconstructed signal
 */
arr_real istft(const std::vector<arr_cmplx>& x, const arr_real& win, int overlap, int nfft,
               StftRange range = StftRange::Onesided, OverlapMethod method = OverlapMethod::Wola);

//win: hann(nfft, `periodic`)
//overlap: nfft/2
arr_real istft(const std::vector<arr_cmplx>& x, int nfft, StftRange range = StftRange::Onesided,
               OverlapMethod method = OverlapMethod::Wola);

}   // namespace dsplib