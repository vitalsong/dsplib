#include "dsplib/subband.h"
#include "dsplib/throw.h"
#include "dsplib/resample.h"
#include "dsplib/fft.h"
#include "dsplib/ifft.h"

// original FilterBanks sources: `https://github.com/kkumatani/distant_speech_recognition`

// details: Digital Receivers and Transmitters Using Polyphase Filter Banks for Wireless Communications, 2003

namespace dsplib {

namespace {

class MatView
{
public:
    explicit MatView(const arr_real& x, int n, int m)
      : n_{n}
      , m_{m}
      , data_{x.data()} {
        DSPLIB_ASSERT(n * m == x.size(), "matrix size error");
    }

    //TODO: use span
    const real_t* operator[](uint32_t i) const noexcept {
        assert(i <= n_);
        return data_ + (i * m_);
    }

private:
    const int n_;
    const int m_;
    const real_t* data_;
};

class MutMatView
{
public:
    explicit MutMatView(arr_real& x, int n, int m)
      : n_{n}
      , m_{m}
      , data_{x.data()} {
        DSPLIB_ASSERT(n * m == x.size(), "matrix size error");
    }

    const real_t* operator[](uint32_t i) const noexcept {
        assert(i <= n_);
        return data_ + (i * m_);
    }

    real_t* operator[](uint32_t i) noexcept {
        assert(i <= n_);
        return data_ + (i * m_);
    }

private:
    const int n_;
    const int m_;
    real_t* data_;
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

    //TODO: use span
    const real_t* operator[](int time) const noexcept {
        time = index_(time);
        return smps_[time];
    }

    real_t* operator[](int time) noexcept {
        time = index_(time);
        return smps_[time];
    }

    void push(const arr_real& s, bool reverse = false) {
        DSPLIB_ASSERT(s.size() == len_, "input size error");
        sample_idx_ = (sample_idx_ + 1) % period_;
        auto* next_frame = smps_[sample_idx_];
        assert(s.size() == len_);
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

protected:
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

    CircBuffer buf_;
    CircBuffer gsi_;
};

arr_real _design_filter(int num_bands, int num_taps) {
    //TODO: cache last coeffs
    return design_multirate_fir(1, num_bands, std::ceil(num_taps / 2.0));
}

}   // namespace

//--------------------------------------------------------------------------------------------------------
class ChannelizerImpl : protected DFTFilterBank
{
public:
    explicit ChannelizerImpl(std::shared_ptr<const arr_real> filter, int num_bands, int decim_factor, int num_taps)
      : DFTFilterBank(num_bands, decim_factor, num_taps, false)
      , flt_{std::move(filter)}
      , fview_{MatView(*flt_, num_taps, num_bands)}
      , ifft_{num_bands} {
    }

    [[nodiscard]] arr_cmplx process(const arr_real& x) {
        DSPLIB_ASSERT(x.size() == d_, "input vector size error");

        gsi_.push(x);

        arr_real convert(nbands_);
        for (int i = 0; i < decim_; i++) {
            const auto* gsi = gsi_[decim_ - i - 1];
            for (int k = 0; k < d_; ++k) {
                convert[k + i * d_] = gsi[k];
            }
        }

        //TODO: move or use `convert` array
        buf_.push(convert, true);

        // calculate outputs of polyphase filters
        arr_real pout(nbands_);
        for (int k = 0; k < ntaps_; k++) {
            const auto* restrict buf = buf_[decim_ * k];
            const auto* restrict flt = fview_[k];
            for (int m = 0; m < nbands_; m++) {
                pout[m] += flt[m] * buf[m];
            }
        }

        //TODO: use fft? pout is real
        return ifft_(pout) * nbands_;
    }

private:
    std::shared_ptr<const arr_real> flt_;
    MatView fview_;
    IfftPlan ifft_;
};

//--------------------------------------------------------------------------------------------------------------
class ChannelSynthesizerImpl : private DFTFilterBank
{
public:
    explicit ChannelSynthesizerImpl(std::shared_ptr<const arr_real> filter, int num_bands, int decim_factor,
                                    int num_taps)
      : DFTFilterBank(num_bands, decim_factor, num_taps, true)
      , flt_{std::move(filter)}
      , fview_{MatView(*flt_, num_taps, num_bands)}
      , fft_{num_bands} {
        //nothing to do
    }

    arr_real process(const dsplib::arr_cmplx& x) {
        DSPLIB_ASSERT(x.size() == nbands_, "input vector size error");

        //TODO: use ifft?
        buf_.push(real(fft_(x)));

        // calculate outputs of polyphase filters
        // TODO: alternative impl for ntaps > nbands
        arr_real convert(nbands_);
        for (int k = 0; k < ntaps_; k++) {
            const auto* restrict buf = buf_[decim_ * k];
            const auto* restrict flt = fview_[k];
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
            const auto* gsi = gsi_[decim_ - i - 1];
            for (int k = 0; k < d_; k++) {
                out[d_ - k - 1] += gsi[k + i * d_];
            }
        }
        return out;
    }

private:
    std::shared_ptr<const arr_real> flt_;
    MatView fview_;
    FftPlan fft_;
};

//--------------------------------------------------------------------------------------------------------------
Channelizer::Channelizer(const arr_real& filter, int num_bands, int decim_factor, int num_taps) {
    const auto fptr = std::make_shared<dsplib::arr_real>(filter);
    d_ = std::make_unique<ChannelizerImpl>(fptr, num_bands, decim_factor, num_taps);
}

Channelizer::Channelizer(std::shared_ptr<const arr_real> filter, int num_bands, int decim_factor, int num_taps) {
    d_ = std::make_unique<ChannelizerImpl>(std::move(filter), num_bands, decim_factor, num_taps);
}

Channelizer::Channelizer(int num_bands, int decim_factor, int num_taps)
  : Channelizer(_design_filter(num_bands, num_taps), num_bands, decim_factor, num_taps) {
}

arr_cmplx Channelizer::process(const arr_real& x) {
    return d_->process(x);
}

//--------------------------------------------------------------------------------------------------------------
ChannelSynthesizer::ChannelSynthesizer(const arr_real& filter, int num_bands, int decim_factor, int num_taps) {
    const auto fptr = std::make_shared<dsplib::arr_real>(filter);
    d_ = std::make_unique<ChannelSynthesizerImpl>(fptr, num_bands, decim_factor, num_taps);
}

ChannelSynthesizer::ChannelSynthesizer(std::shared_ptr<const arr_real> filter, int num_bands, int decim_factor,
                                       int num_taps) {
    d_ = std::make_unique<ChannelSynthesizerImpl>(std::move(filter), num_bands, decim_factor, num_taps);
}

ChannelSynthesizer::ChannelSynthesizer(int num_bands, int decim_factor, int num_taps)
  : ChannelSynthesizer(_design_filter(num_bands, num_taps), num_bands, decim_factor, num_taps) {
}

arr_real ChannelSynthesizer::process(const arr_cmplx& x) {
    return d_->process(x);
}

}   // namespace dsplib