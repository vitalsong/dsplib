#include <dsplib/mfcc.h>

#include <dsplib/window.h>
#include <dsplib/array.h>
#include <dsplib/fft.h>
#include <dsplib/utils.h>
#include <dsplib/math.h>

namespace dsplib {

namespace {

arr_int _freq2bin(const arr_real& freqs, int nfft, int fs) {
    arr_int bins(freqs.size());
    for (int i = 0; i < freqs.size(); ++i) {
        bins[i] = std::round((freqs[i] / fs) * nfft);
    }
    return bins;
}

//TODO: add to utils
arr_real _linspace(real_t start, real_t end, int count) {
    arr_real out(count);
    const real_t step = (count == 1) ? 0 : (end - start) / real_t(count - 1);
    for (int i = 0; i < count; ++i) {
        out[i] = start + (step * i);
    }
    return out;
}

std::vector<arr_real> _gen_dcttable(int nmels, int nbands) {
    // The DCT-II unitary algorithm
    const real_t A = std::sqrt(1.0 / nbands);
    const real_t B = std::sqrt(2.0 / nbands);
    const real_t piv = pi / nbands;
    std::vector<arr_real> mat(nmels, ones(nbands) * A);
    for (int k = 0; k < nbands; ++k) {
        for (int n = 1; n < nmels; ++n) {
            mat[n][k] = B * std::cos(piv * n * (k + 0.5));
        }
    }
    return mat;
}

}   // namespace

arr_real MFCCExtractor::design_band_edges(int min_freq, int max_freq, int nbands) {
    const auto min_mel = hz2mel(real_t(min_freq));
    const auto max_mel = hz2mel(real_t(max_freq));
    const auto mels = _linspace(min_mel, max_mel, nbands + 2);
    const auto freqs = mel2hz(mels);
    return freqs;
}

std::vector<arr_real> MFCCExtractor::design_filterbank(int fs, int min_freq, int max_freq, int nbands, int nfft) {
    const auto band_edges = design_band_edges(min_freq, max_freq, nbands);
    const auto bins = _freq2bin(band_edges, nfft, fs) + 1;   //matlab-style indexies
    const int nb = band_edges.size();
    const auto band_width = *band_edges.slice(2, nb) - *band_edges.slice(0, nb - 2);
    const auto weight_bands = band_width / 2;

    std::vector<arr_real> filterbank;
    for (int en = 0; en < nbands; ++en) {
        arr_real flt(nfft);

        // rising side of triangle
        for (int i = bins[en]; i <= bins[en + 1]; ++i) {
            flt[i - 1] = real_t(i - bins[en]) / (bins[en + 1] - bins[en]);
        }

        // falling side of triangle
        for (int i = bins[en + 1] + 1; i <= bins[en + 2]; ++i) {
            flt[i - 1] = 1.0 - real_t(i - bins[en + 1]) / (bins[en + 2] - bins[en + 1]);
        }

        //normalization
        flt /= weight_bands(en);

        filterbank.push_back(flt);
    }

    return filterbank;
}

MFCCExtractor::MFCCExtractor(int fs, int nfft, int overlap, int nmels, int nbands, const arr_real& win)
  : winlen_{win.size()}
  , win_{win}
  , hop_size_{winlen_ - overlap}
  , nmels_{nmels}
  , nfft_{nfft}
  , fbank_{design_filterbank(fs, 0, fs / 2, nbands, nfft)}
  , dctmat_{_gen_dcttable(nmels, nbands)}
  , buf_(winlen_) {
}

arr_real MFCCExtractor::process(const arr_real& x) {
    DSPLIB_ASSERT(x.size() == hop_size_, "input size must be equal hop_size");

    buf_.slice(0, winlen_ - hop_size_) = buf_.slice(hop_size_, winlen_);
    buf_.slice(winlen_ - hop_size_, winlen_) = x;
    const arr_real xq = abs2(fft(buf_ * win_, nfft_));

    //filterbank
    //TODO: use only nfft/2+1 points for real signal
    //TODO: store and multiply only small non-zero part of filterbank - (fftpos, fbank)
    arr_real z(fbank_.size());
    for (int i = 0; i < fbank_.size(); ++i) {
        z[i] = dot(fbank_[i], xq);
    }

    //cepstral
    z = log10(z);
    arr_real y(nmels_);
    for (int i = 0; i < nmels_; ++i) {
        y[i] = dot(z, dctmat_[i]);
    }

    return y;
}

}   // namespace dsplib