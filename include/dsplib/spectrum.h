#pragma once

#include "dsplib/array.h"
#include "dsplib/keywords.h"

namespace dsplib {

//Welch’s power spectral density estimate
//win: nfft size window
//nfft: number of DFT points (only 2^K support)
//type: power spectrum scaling (dencity/power)

//note: PSD unit is `Hz`, the `fs` is assumed to be 1.0
//todo: add min/max/mean/median average

struct WelchResult
{
    WelchResult(const arr_real& pxx_, const arr_real& f_)
      : pxx{pxx_}
      , f{f_} {
    }
    arr_real pxx;   ///< spectrum estimate
    arr_real f;     ///< cyclical frequencies
};

//real spectrum (one-sided)

//use hamming window with winlen/2 overlaping
WelchResult welch(const arr_real& x, int winlen, SpectrumType scale = SpectrumType::Psd);

//use custom window with winlen/2 overlaping
WelchResult welch(const arr_real& x, const arr_real& win, SpectrumType scale = SpectrumType::Psd);

WelchResult welch(const arr_real& x, int winlen, int noverlap, int nfft, SpectrumType scale = SpectrumType::Psd);

WelchResult welch(const arr_real& x, const arr_real& win, int noverlap, int nfft,
                  SpectrumType scale = SpectrumType::Psd);

//complex spectrum (two-sided centered)

WelchResult welch(const arr_cmplx& x, int winlen, SpectrumType scale = SpectrumType::Psd);

WelchResult welch(const arr_cmplx& x, const arr_real& win, SpectrumType scale = SpectrumType::Psd);

WelchResult welch(const arr_cmplx& x, int winlen, int noverlap, int nfft, SpectrumType scale = SpectrumType::Psd);

WelchResult welch(const arr_cmplx& x, const arr_real& win, int noverlap, int nfft,
                  SpectrumType scale = SpectrumType::Psd);

}   // namespace dsplib