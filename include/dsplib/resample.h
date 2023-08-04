#pragma once

#include <memory>

#include <dsplib/utils.h>
#include <dsplib/array.h>
#include <dsplib/math.h>

namespace dsplib {

//TODO: complex type implementation

//multirate FIR filter design (similar to implementation in MATLAB)
arr_real design_multirate_fir(int interp, int decim, int hlen = 12, real_t astop = 80);

//------------------------------------------------------------------------------
//base resample class
class IResampler
{
public:
    virtual ~IResampler() = default;

    virtual arr_real process(const arr_real& sig) = 0;

    [[nodiscard]] virtual int delay() const noexcept {
        return 0;
    }

    [[nodiscard]] virtual int decim_rate() const noexcept {
        return 1;
    }

    [[nodiscard]] virtual int interp_rate() const noexcept {
        return 1;
    }

    [[nodiscard]] int next_size(int size) const noexcept {
        return IResampler::next_size(size, this->interp_rate(), this->decim_rate());
    }

    [[nodiscard]] int prev_size(int size) const noexcept {
        return IResampler::prev_size(size, this->interp_rate(), this->decim_rate());
    }

    //polyphase decomposition of multirate filter
    static std::vector<arr_real> polyphase(arr_real h, int m, real_t gain = 1.0);

    //nearest multiple of frame size to process
    static int next_size(int size, int p, int q);
    static int prev_size(int size, int p, int q);
};

//------------------------------------------------------------------------------
//polyphase FIR decimation
class FIRDecimator : public IResampler
{
public:
    explicit FIRDecimator(int decim);

    //decim - decimation factor
    //h - custom multirate fir filter
    explicit FIRDecimator(int decim, const arr_real& h);

    //in.size() must be a multiple of the decim
    arr_real process(const arr_real& in) final;

    [[nodiscard]] int delay() const noexcept final;
    [[nodiscard]] int decim_rate() const noexcept final;

private:
    std::vector<arr_real> h_;
    arr_real d_;
    int decim_;
    int sublen_;
};

//------------------------------------------------------------------------------
//polyphase FIR interpolation
class FIRInterpolator : public IResampler
{
public:
    explicit FIRInterpolator(int interp);

    //interp - interpolation factor
    //h - custom multirate fir filter
    explicit FIRInterpolator(int interp, const arr_real& h);

    arr_real process(const arr_real& in) final;

    [[nodiscard]] int delay() const noexcept final;
    [[nodiscard]] int interp_rate() const noexcept final;

private:
    std::vector<arr_real> h_;
    arr_real d_;
    int interp_;
    int sublen_;
};

//------------------------------------------------------------------------------
//polyphase FIR sample rate conversion
class FIRRateConverter : public IResampler
{
public:
    explicit FIRRateConverter(int interp, int decim);

    //interp - interpolation factor
    //decim - decimation factor
    //h - custom multirate fir filter
    explicit FIRRateConverter(int interp, int decim, const arr_real& h);

    //in.size() must be a multiple of the decim
    arr_real process(const arr_real& in) final;

    [[nodiscard]] int delay() const noexcept final;
    [[nodiscard]] int interp_rate() const noexcept final;
    [[nodiscard]] int decim_rate() const noexcept final;

private:
    std::vector<arr_real> h_;
    arr_real d_;
    int interp_;
    int decim_;
    int sublen_;
};

//------------------------------------------------------------------------------
//Wrapper over FIRRateConverter, FIRInterpolator and FIRDecimator
//with calculation of optimal L/M coefficients for sample rate conversion
//TODO: filter transition band setting
class FIRResampler : public IResampler
{
public:
    //out_fs - output sample rate (Hz)
    //in_fs - input sample rate (Hz)
    explicit FIRResampler(int out_fs, int in_fs);

    enum class Mode
    {
        Bypass,
        Decimator,
        Interpolator,
        Resampler
    };

    arr_real process(const arr_real& sig) final;

    [[nodiscard]] int delay() const noexcept final;
    [[nodiscard]] int interp_rate() const noexcept final;
    [[nodiscard]] int decim_rate() const noexcept final;

private:
    Mode mode_{Mode::Bypass};
    std::shared_ptr<IResampler> rsmp_;
};

//------------------------------------------------------------------------------
//resamples the input sequence, x, at p/q times the original sample rate
//as p/q coefficients, you can use out_fs/in_fs
arr_real resample(const arr_real& x, int p, int q);

}   // namespace dsplib