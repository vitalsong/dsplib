#include "dsplib/resample.h"

namespace dsplib {

namespace {

class Decimator : public IResampler
{
public:
    explicit Decimator(int decim, span_real h)
      : decim_{decim}
      , h_{polyphase(h, decim_, 1.0, false)}
      , flen_{h_[0].size()}   //TODO: flen can be const for typical `design_multirate_fir`
      , d_(decim_ * (flen_ - 1)) {
        dH_ = (h.size() - 1) / 2;
    }

    arr_real process(span_real in) final {
        const int nx = in.size();
        const int nd = d_.size();
        const int ny = nx / decim_;

        DSPLIB_ASSERT(nx % decim_ == 0, "input frame length must be a multiple of the 'decim'");

        arr_real x(nd + nx);
        std::memcpy(x.data(), d_.data(), nd * sizeof(real_t));
        std::memcpy(x.data() + nd, in.data(), nx * sizeof(real_t));
        std::memcpy(d_.data(), x.data() + nx, nd * sizeof(real_t));

        arr_real y(ny);
        if (decim_ == 2) {
            _decimate_d2(x.data(), y.data(), ny);
        } else if (decim_ == 3) {
            _decimate_d3(x.data(), y.data(), ny);
        } else if (decim_ == 4) {
            _decimate_d4(x.data(), y.data(), ny);
        } else {
            _decimate(x.data(), y.data(), ny);
        }

        return y;
    }

    [[nodiscard]] int delay() const noexcept final {
        //TODO: return float value
        return round(dH_ / decim_);
    }

    [[nodiscard]] int decim_rate() const noexcept final {
        return decim_;
    }

private:
    void _decimate(const real_t* restrict x, real_t* restrict y, int ny) noexcept {
        for (int i = 0; i < ny; ++i) {
            const real_t* restrict px = x + (decim_ * i);
            real_t acc = 0;
            for (int j = 0; j < flen_; ++j) {
                for (int k = 0; k < decim_; ++k) {
                    acc += px[k] * h_[k][j];
                }
                px += decim_;
            }
            y[i] = acc;
        }
    }

    //it can be 2-3x faster
    void _decimate_d2(const real_t* restrict x, real_t* restrict y, int ny) noexcept {
        constexpr int D = 2;
        const real_t* restrict h0 = h_[0].data();
        const real_t* restrict h1 = h_[1].data();
        for (int i = 0; i < ny; ++i) {
            const real_t* restrict px = x + (D * i);
            real_t acc = 0;
            for (int j = 0; j < flen_; ++j) {
                acc += px[0] * h0[j] + px[1] * h1[j];
                px += D;
            }
            y[i] = acc;
        }
    }

    void _decimate_d3(const real_t* restrict x, real_t* restrict y, int ny) noexcept {
        constexpr int D = 3;
        const real_t* restrict h0 = h_[0].data();
        const real_t* restrict h1 = h_[1].data();
        const real_t* restrict h2 = h_[2].data();
        for (int i = 0; i < ny; ++i) {
            const real_t* restrict px = x + (D * i);
            real_t acc = 0;
            for (int j = 0; j < flen_; ++j) {
                acc += px[0] * h0[j] + px[1] * h1[j] + px[2] * h2[j];
                px += D;
            }
            y[i] = acc;
        }
    }

    void _decimate_d4(const real_t* restrict x, real_t* restrict y, int ny) noexcept {
        constexpr int D = 4;
        const real_t* restrict h0 = h_[0].data();
        const real_t* restrict h1 = h_[1].data();
        const real_t* restrict h2 = h_[2].data();
        const real_t* restrict h3 = h_[3].data();
        for (int i = 0; i < ny; ++i) {
            const real_t* restrict px = x + (D * i);
            real_t acc = 0;
            for (int j = 0; j < flen_; ++j) {
                acc += px[0] * h0[j] + px[1] * h1[j] + px[2] * h2[j] + px[3] * h3[j];
                px += D;
            }
            y[i] = acc;
        }
    }

    const int decim_;
    std::vector<arr_real> h_;
    const int flen_;
    arr_real d_;
    real_t dH_{0};
};

}   // namespace

FIRDecimator::FIRDecimator(int decim)
  : FIRDecimator(decim, design_multirate_fir(1, decim)) {
}

FIRDecimator::FIRDecimator(int decim, span_real h) {
    d_ = std::make_shared<Decimator>(decim, h);
}

arr_real FIRDecimator::process(span_real in) {
    return d_->process(in);
}

[[nodiscard]] int FIRDecimator::delay() const noexcept {
    return d_->delay();
}

[[nodiscard]] int FIRDecimator::decim_rate() const noexcept {
    return d_->decim_rate();
}

}   // namespace dsplib
