#include "dsplib/subband.h"
#include "dsplib/assert.h"
#include "dsplib/fir.h"
#include "dsplib/fft.h"
#include "dsplib/ifft.h"

// original FilterBanks sources: `https://github.com/kkumatani/distant_speech_recognition`

// details: Digital Receivers and Transmitters Using Polyphase Filter Banks for Wireless Communications, 2003

namespace dsplib {

namespace {

class MatView
{
public:
    explicit MatView(span_t<real_t> x, int n, int m)
      : n_{n}
      , m_{m}
      , data_{x} {
        DSPLIB_ASSERT(n * m == x.size(), "Matrix size error");
    }

    span_t<real_t> operator[](int i) const noexcept {
        DSPLIB_ASSUME(i >= 0 && i < n_);
        const int i1 = i * m_;
        const int i2 = i1 + m_;
        return data_.slice(i1, i2);
    }

private:
    const int n_{0};
    const int m_{0};
    span_t<real_t> data_;
};

class MutMatView
{
public:
    explicit MutMatView(mut_span_t<real_t> x, int n, int m)
      : n_{n}
      , m_{m}
      , data_{x} {
        DSPLIB_ASSERT(n * m == x.size(), "Matrix size error");
    }

    span_t<real_t> operator[](int i) const noexcept {
        DSPLIB_ASSUME(i >= 0 && i < n_);
        const int i1 = i * m_;
        const int i2 = i1 + m_;
        return data_.slice(i1, i2);
    }

    mut_span_t<real_t> operator[](int i) noexcept {
        DSPLIB_ASSUME(i >= 0 && i < n_);
        const int i1 = i * m_;
        const int i2 = i1 + m_;
        return data_.slice(i1, i2);
    }

private:
    const int n_{0};
    const int m_{0};
    mut_span_t<real_t> data_;
};

class CircBuffer
{
public:
    /**
     * @brief Construct a circular buffer to keep samples periodically
     * @details It keeps period arrays which is completely updated with the period 'period'.
     * Each array holds actual values of the samples.
     * @param len The size of each array
     * @param period The period of the circular buffer
     */
    explicit CircBuffer(int len, int period)
      : len_{len}
      , period_{period}
      , sample_idx_{period_ - 1}
      , buf_(period_ * len_)
      , smps_{buf_, period_, len_} {
    }

    span_t<real_t> operator[](int time) const noexcept {
        time = index_(time);
        return smps_[time];
    }

    mut_span_t<real_t> operator[](int time) noexcept {
        time = index_(time);
        return smps_[time];
    }

    void push(span_real s, bool reverse = false) {
        DSPLIB_ASSERT(s.size() == len_, "input size error");
        sample_idx_ = (sample_idx_ + 1) % period_;
        auto next_frame = smps_[sample_idx_];
        if (reverse) {
            for (int i = 0; i < len_; i++) {
                next_frame[i] = s[len_ - i - 1];
            }
        } else {
            for (int i = 0; i < len_; i++) {
                next_frame[i] = s[i];
            }
        }
    }

private:
    [[nodiscard]] int index_(int idx) const noexcept {
        assert(idx < period_);
        //TODO: index optimization
        return (sample_idx_ + period_ - idx) % period_;
    }

    const int len_;
    const int period_;
    int sample_idx_;   // index of most recent sample
    arr_real buf_;
    MutMatView smps_;
};

class DFTFilterBank
{
public:
    virtual ~DFTFilterBank() = default;

    explicit DFTFilterBank(int num_bands, int decim_factor, int num_taps, bool synthesis = false)
      : nbands_{num_bands}
      , ntaps_{num_taps}
      , decim_{decim_factor}
      , d_{nbands_ / decim_}
      , buf_{nbands_, num_taps * decim_}
      , gsi_{(synthesis ? nbands_ : d_), decim_} {
    }

    const int nbands_;
    const int ntaps_;
    const int decim_;
    const int d_;

    //TODO: size optimization
    CircBuffer buf_;
    CircBuffer gsi_;
};

arr_real _design_filter(int num_bands, int num_taps) {
    //TODO: cache last coeffs
    auto h = fir1(num_bands * num_taps - 1, 1.0 / num_bands, FilterType::Low);
    return h;
}

}   // namespace

//--------------------------------------------------------------------------------------------------------
class ChannelizerImpl : public DFTFilterBank
{
public:
    explicit ChannelizerImpl(std::shared_ptr<const arr_real> filter, int num_bands, int decim_factor, int num_taps)
      : DFTFilterBank(num_bands, decim_factor, num_taps, false)
      , flt_{std::move(filter)}
      , fview_{MatView(*flt_, num_taps, num_bands)}
      , fft_{fft_plan_r(num_bands)} {
    }

    [[nodiscard]] arr_cmplx process(span_real x) {
        DSPLIB_ASSERT(x.size() == d_, "input vector size error");

        gsi_.push(x);

        arr_real convert(nbands_);
        for (int i = 0; i < decim_; i++) {
            auto gsi = gsi_[decim_ - i - 1];
            for (int k = 0; k < d_; ++k) {
                convert[k + i * d_] = gsi[k];
            }
        }
        buf_.push(convert, true);

        // calculate outputs of polyphase filters
        arr_real pout(nbands_);
        for (int k = 0; k < ntaps_; k++) {
            auto buf = buf_[decim_ * k];
            auto flt = fview_[k];
            for (int m = 0; m < nbands_; m++) {
                pout[m] += flt[m] * buf[m];
            }
        }

        //TODO: flip and remove conj
        auto out = fft_->solve(pout);
        conj(inplace(out));
        return out;
    }

private:
    std::shared_ptr<const arr_real> flt_;
    MatView fview_;
    std::shared_ptr<FftPlanR> fft_;
};

//--------------------------------------------------------------------------------------------------------------
class ChannelSynthesizerImpl : public DFTFilterBank
{
public:
    explicit ChannelSynthesizerImpl(std::shared_ptr<const arr_real> filter, int num_bands, int decim_factor,
                                    int num_taps)
      : DFTFilterBank(num_bands, decim_factor, num_taps, true)
      , flt_{std::move(filter)}
      , fview_{MatView(*flt_, num_taps, num_bands)}
      , ifft_{ifft_plan_r(num_bands)} {
    }

    arr_real process(span_cmplx x) {
        DSPLIB_ASSERT(x.size() == nbands_, "input vector size error");

        auto xx = ifft_->solve(x);
        xx *= nbands_;

        //TODO: fft and flip?
        buf_.push(xx, true);

        // calculate outputs of polyphase filters
        // TODO: alternative impl for ntaps > nbands
        arr_real convert(nbands_);
        for (int k = 0; k < ntaps_; k++) {
            auto buf = buf_[decim_ * k];
            auto flt = fview_[k];
            for (int m = 0; m < nbands_; m++) {
                //TODO: inverse filter order
                convert[m] += flt[nbands_ - m - 1] * buf[m];
            }
        }
        gsi_.push(convert);

        // synthesize final output of filterbank
        // TODO: inverse order?
        arr_real out(d_);
        for (int i = 0; i < decim_; i++) {
            auto gsi = gsi_[decim_ - i - 1];
            for (int k = 0; k < d_; k++) {
                out[d_ - k - 1] += gsi[k + i * d_];
            }
        }

        //normalize ouput
        //TODO: more precision, gain error ~ 1 dB
        out *= nbands_ / decim_;
        return out;
    }

private:
    std::shared_ptr<const arr_real> flt_;
    MatView fview_;
    std::shared_ptr<IfftPlanR> ifft_;
};

//--------------------------------------------------------------------------------------------------------------
Channelizer::Channelizer(span_real filter, int num_bands, int decim_factor)
  : Channelizer(std::make_shared<dsplib::arr_real>(filter), num_bands, decim_factor) {
}

Channelizer::Channelizer(std::shared_ptr<const arr_real> filter, int num_bands, int decim_factor) {
    const int num_taps = filter->size() / num_bands;
    DSPLIB_ASSERT(num_taps % 2 == 0, "`num_taps` expected to be even");
    DSPLIB_ASSERT(num_bands % decim_factor == 0, "only integer ratio M/D supported");
    DSPLIB_ASSERT(filter->size() == num_bands * num_taps, "filter size must be equal `num_bands * num_taps`");
    d_ = std::make_unique<ChannelizerImpl>(std::move(filter), num_bands, decim_factor, num_taps);
}

Channelizer::Channelizer(int num_bands, int decim_factor, int num_taps)
  : Channelizer(_design_filter(num_bands, num_taps), num_bands, decim_factor) {
}

arr_cmplx Channelizer::process(span_real x) {
    return d_->process(x);
}

int Channelizer::frame_len() const noexcept {
    return (d_->nbands_ / d_->decim_);
}

//--------------------------------------------------------------------------------------------------------------
ChannelSynthesizer::ChannelSynthesizer(span_real filter, int num_bands, int decim_factor)
  : ChannelSynthesizer(std::make_shared<dsplib::arr_real>(filter), num_bands, decim_factor) {
}

ChannelSynthesizer::ChannelSynthesizer(std::shared_ptr<const arr_real> filter, int num_bands, int decim_factor) {
    const int num_taps = filter->size() / num_bands;
    DSPLIB_ASSERT(num_taps % 2 == 0, "`num_taps` expected to be even");
    DSPLIB_ASSERT(num_bands % decim_factor == 0, "only integer ratio M/D supported");
    DSPLIB_ASSERT(filter->size() == num_bands * num_taps, "filter size must be equal `num_bands * num_taps`");
    d_ = std::make_unique<ChannelSynthesizerImpl>(std::move(filter), num_bands, decim_factor, num_taps);
}

ChannelSynthesizer::ChannelSynthesizer(int num_bands, int decim_factor, int num_taps)
  : ChannelSynthesizer(_design_filter(num_bands, num_taps), num_bands, decim_factor) {
}

arr_real ChannelSynthesizer::process(span_cmplx x) {
    return d_->process(x);
}

int ChannelSynthesizer::frame_len() const noexcept {
    return (d_->nbands_ / d_->decim_);
}

}   // namespace dsplib