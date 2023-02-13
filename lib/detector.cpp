#include <dsplib/detector.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

#include <cassert>

namespace dsplib {

//---------------------------------------------------------------------------------
Detector::Detector(const arr_cmplx& ref, real_t threshold)
  : nh_{ref.size()}
  , fft_{int(1) << nextpow2(2 * nh_)}
  , buf_(fft_.size())
  , threshold_{threshold} {
    assert((threshold > 0) && (threshold < 1.0));
    ref_ = conj(fft_(zeropad(ref, fft_.size())));
}

//---------------------------------------------------------------------------------
Detector::State Detector::process(const arr_cmplx& sig) {
    Detector::State result;
    result.out = sig;

    int offset = 0;
    const int nfft = fft_.size();
    for (int k = 0; k < sig.size(); ++k) {
        buf_[wpos_ + nfft / 2] = sig[k];
        result.out[k] = buf_[wpos_];
        wpos_++;
        offset++;
        if (wpos_ == nfft / 2) {
            const auto X1 = fft_(buf_);
            auto Y = X1 * ref_;
            for (int i = 0; i < nfft; ++i) {
                Y[i] = conj(Y[i]) / abs(Y[i]);
            }
            const auto R = fft_(Y) / nfft;
            const auto peak_idx = argmax(R);
            const auto peak_val = abs(R[peak_idx]);
            if ((peak_val > threshold_) && (peak_idx < nfft / 2)) {
                result.triggered = true;
                result.level = peak_val;
                result.position = offset + peak_idx - (nfft / 2);
            }

            buf_.slice(0, nfft / 2) = buf_.slice(nfft / 2, nfft);
            wpos_ = 0;
        }
    }

    return result;
}

}   // namespace dsplib