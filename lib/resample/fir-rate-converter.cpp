#include "dsplib/resample.h"
#include "dsplib/throw.h"

namespace dsplib {

FIRRateConverter::FIRRateConverter(int interp, int decim)
  : FIRRateConverter{interp, decim, design_multirate_fir(interp, decim)} {
}

FIRRateConverter::FIRRateConverter(int interp, int decim, const arr_real& h)
  : interp_{interp}
  , decim_{decim} {
    h_ = polyphase(h, interp_, interp_);
    sublen_ = h_[0].size();
    d_ = zeros(sublen_ - 1);
}

arr_real FIRRateConverter::process(const arr_real& in) {
    const int nx = in.size();
    const int nh = sublen_;
    const int nd = d_.size();

    DSPLIB_ASSERT(nx % decim_ == 0, "Input frame length must be a multiple of the 'decim'");

    arr_real x(nd + nx);
    std::memcpy(x.data(), d_.data(), nd * sizeof(real_t));
    std::memcpy(x.data() + nd, in.data(), nx * sizeof(real_t));
    std::memcpy(d_.data(), x.data() + nx, nd * sizeof(real_t));

    auto y = zeros(nx * interp_ / decim_);
    auto* py = y.data();
    int st = 0;
    for (int i = 0; i < nx; ++i) {
        for (int k = 0; k < interp_; ++k) {
            ++st;
            if (st == decim_) {
                const auto& h = h_[k];
                for (int j = 0; j < nh; ++j) {
                    //TODO: check for non-symmetry IR
                    *py += x[i + j] * h[nh - j - 1];
                }
                ++py;
                st = 0;
            }
        }
    }
    return y;
}

int FIRRateConverter::delay() const noexcept {
    //TODO: must be N/2
    return sublen_ / 2 + 1;
}

int FIRRateConverter::interp_rate() const noexcept {
    return interp_;
}

[[nodiscard]] int FIRRateConverter::decim_rate() const noexcept {
    return decim_;
}

}   // namespace dsplib