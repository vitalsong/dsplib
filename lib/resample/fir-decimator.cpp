#include "dsplib/resample.h"
#include "dsplib/throw.h"

namespace dsplib {

FIRDecimator::FIRDecimator(int decim)
  : FIRDecimator(decim, design_multirate_fir(1, decim)) {
}

FIRDecimator::FIRDecimator(int decim, const arr_real& h)
  : decim_{decim} {
    h_ = polyphase(h, decim_, 1.0, false);
    sublen_ = h_[0].size();
    d_ = zeros(decim_ * (sublen_ - 1));
}

arr_real FIRDecimator::process(const arr_real& in) {
    const int nx = in.size();
    const int nd = d_.size();

    DSPLIB_ASSERT(nx % decim_ == 0, "Input frame length must be a multiple of the 'decim'");

    arr_real x(nd + nx);
    std::memcpy(x.data(), d_.data(), nd * sizeof(real_t));
    std::memcpy(x.data() + nd, in.data(), nx * sizeof(real_t));
    std::memcpy(d_.data(), x.data() + nx, nd * sizeof(real_t));

    auto y = dsplib::zeros(nx / decim_);
    const auto* px = x.data();
    for (int i = 0; i < y.size(); ++i) {
        for (int k = 0; k < decim_; ++k) {
            const auto& h = h_[k];
            //TODO: index optimization
            for (int j = 0, idx = k; j < sublen_; ++j, idx += decim_) {
                //TODO: check for non-symmetry IR
                y[i] += px[idx] * h[j];
            }
        }
        px += decim_;
    }
    return y;
}

[[nodiscard]] int FIRDecimator::delay() const noexcept {
    return sublen_ / 2;
}

[[nodiscard]] int FIRDecimator::decim_rate() const noexcept {
    return decim_;
}

}   // namespace dsplib
