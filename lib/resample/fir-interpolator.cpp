#include "dsplib/array.h"
#include "dsplib/resample.h"

namespace dsplib {

FIRInterpolator::FIRInterpolator(int interp)
  : FIRInterpolator{interp, design_multirate_fir(interp, 1)} {
}

FIRInterpolator::FIRInterpolator(int interp, const arr_real& h)
  : interp_{interp} {
    h_ = polyphase(h, interp_, real_t(interp_), true);
    sublen_ = h_[0].size();
    d_ = zeros(sublen_ - 1);
}

arr_real FIRInterpolator::process(const arr_real& in) {
    const int nx = in.size();
    const int nh = sublen_;
    const int nd = d_.size();

    arr_real px(nd + nx);
    std::memcpy(px.data(), d_.data(), nd * sizeof(real_t));
    std::memcpy(px.data() + nd, in.data(), nx * sizeof(real_t));
    std::memcpy(d_.data(), px.data() + nx, nd * sizeof(real_t));

    auto y = arr_real(nx * interp_);
    auto* py = y.data();
    for (int i = 0; i < nx; ++i) {
        for (int k = 0; k < interp_; ++k, ++py) {
            for (int j = 0; j < nh; ++j) {
                *py += px[i + j] * h_[k][j];
            }
        }
    }
    return y;
}

int FIRInterpolator::delay() const noexcept {
    return (sublen_ * interp_) / 2;
}

int FIRInterpolator::interp_rate() const noexcept {
    return interp_;
}

}   // namespace dsplib