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

//Short Time Fourier Transform
std::vector<arr_cmplx> stft(const arr_real& x, const arr_real& win, int overlap, int nfft,
                            StftRange range = StftRange::Onesided);

//win: hann(nfft, `periodic`)
//overlap: nfft/2
std::vector<arr_cmplx> stft(const arr_real& x, int nfft, StftRange range = StftRange::Onesided);

//Inverse Short Time Fourier Transform
arr_real istft(const std::vector<arr_cmplx>& x, const arr_real& win, int overlap, int nfft,
               StftRange range = StftRange::Onesided, OverlapMethod method = OverlapMethod::Wola);

//win: hann(nfft, `periodic`)
//overlap: nfft/2
arr_real istft(const std::vector<arr_cmplx>& x, int nfft, StftRange range = StftRange::Onesided,
               OverlapMethod method = OverlapMethod::Wola);

}   // namespace dsplib