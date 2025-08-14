#include "dsplib/resample.h"

#include <cassert>

namespace dsplib {

FIRRateConverter::FIRRateConverter(int interp, int decim)
  : FIRRateConverter{interp, decim, design_multirate_fir(interp, decim)} {
}

FIRRateConverter::FIRRateConverter(int interp, int decim, span_real h)
  : interp_{interp}
  , decim_{decim} {
    const auto th = polyphase(h, interp_, real_t(interp_), true);
    sublen_ = th[0].size();
    d_ = zeros(sublen_ - 1);

    //polyphase table access optimization
    //example, for interp=3, decim=5 the processed brunches are (1 0 2)
    int st = 0;
    xidxs_.reserve(interp_);
    for (int i = 0; i < decim_; ++i) {
        for (int k = 0; k < interp_; ++k) {
            st = st + 1;
            if (st == decim_) {
                h_.emplace_back(th[k]);
                xidxs_.push_back(i);   //offset of input signal for each brunch
                st = 0;
            }
        }
    }

    assert(int(h_.size()) == interp_);
    assert(int(xidxs_.size()) == interp_);
}

arr_real FIRRateConverter::process(span_real in) {
    const int nx = in.size();
    const int nh = sublen_;
    const int nd = d_.size();

    DSPLIB_ASSERT(nx % decim_ == 0, "Input frame length must be a multiple of the 'decim'");

    arr_real x(nd + nx);
    std::memcpy(x.data(), d_.data(), nd * sizeof(real_t));
    std::memcpy(x.data() + nd, in.data(), nx * sizeof(real_t));
    std::memcpy(d_.data(), x.data() + nx, nd * sizeof(real_t));

    const int np = nx / decim_;
    auto y = zeros(np * interp_);
    auto* py = y.data();
    for (int i = 0; i < np; ++i) {
        for (int k = 0; k < interp_; ++k) {
            const auto& h = h_[k];
            const auto* px = x.data() + (i * decim_) + xidxs_[k];
            for (int j = 0; j < nh; ++j) {
                *py += px[j] * h[j];
            }
            ++py;
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