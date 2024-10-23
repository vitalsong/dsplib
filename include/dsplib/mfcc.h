#pragma once

#include <dsplib/array.h>

namespace dsplib {

template<typename T>
T hz2mel(const T& freq) noexcept {
    return 2595 * log10(1 + freq / 700);
}

template<typename T>
T mel2hz(const T& mel) noexcept {
    return 700 * (power(10, (mel / 2595)) - 1);
}

class MFCCExtractor
{
public:
    explicit MFCCExtractor(int fs, int nfft, int overlap, int nmels, int nbands, const arr_real& win);

    arr_real process(const arr_real& x);

    static arr_real design_band_edges(int min_freq, int max_freq, int nbands);

    static std::vector<arr_real> design_filterbank(int fs, int min_freq, int max_freq, int nbands, int nfft);

private:
    const int winlen_;
    const arr_real win_;
    const int hop_size_;
    const int nmels_;
    const int nfft_;
    const std::vector<arr_real> fbank_;
    const std::vector<arr_real> dctmat_;
    arr_real buf_;
};

}   // namespace dsplib