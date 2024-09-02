#pragma once

#include <dsplib/array.h>

#include <cassert>
#include <memory>

namespace dsplib {

class ChannelizerImpl;

//TODO: use decim factor in matlab style (M/D)

/**
 * @brief Polyphase FFT analysis filter bank
 * @details Separates a broadband input signal into multiple narrow subbands
 * @see Matlab dsp.Channelizer
 * @todo Add `Range` parameter [OneSided, Twosided, Centered]
 */
class Channelizer
{
public:
    Channelizer(const Channelizer&) = delete;

    /**
     * @brief Construct Channelizer
     * 
     * @param filter Multirate FIR coeffs [num_bands * ntaps]
     * @param num_bands Number of frequency bands
     * @param decim_factor Decimation factor [1 : M-1]
     */
    explicit Channelizer(const arr_real& filter, int num_bands, int decim_factor);

    /**
     * @brief Construct Channelizer
     * @details Use this function to save memory if multiple Channelizer/ChannelSynthesizer objects can exist at the same time
     * @param filter Pointer to multirate FIR coeffs
     * @param num_bands Number of frequency bands
     * @param decim_factor Decimation factor [1 : M-1]
     */
    explicit Channelizer(std::shared_ptr<const arr_real> filter, int num_bands, int decim_factor);

    /**
     * @brief Construct Channelizer
     * @details The filter will be calculated using the `design_multirate_fir(1, num_bands, ceil(num_taps / 2.0))`
     * @param num_bands Number of frequency bands
     * @param decim_factor Decimation factor [1 : M-1]
     * @param num_taps Number of filter coefficients per frequency band (expected to be even)
     */
    explicit Channelizer(int num_bands, int decim_factor, int num_taps);

    /**
     * @brief Filter bank processing (analysis)
     * @param x Input broadband signal [num_bands / decim_factor]
     * @return arr_cmplx Subband signal [num_bands]
     */
    [[nodiscard]] arr_cmplx process(const arr_real& x);

    arr_cmplx operator()(const arr_real& x) {
        return this->process(x);
    }

    /**
     * @brief Processing frame size
     * @return int frame len
     */
    [[nodiscard]] int frame_len() const noexcept;

private:
    std::shared_ptr<ChannelizerImpl> d_;
};

class ChannelSynthesizerImpl;

/**
 * @brief Polyphase FFT synthesis filter bank
 * @details Ideally ChannelSynthesizer(Channelizer(x)) == x.
 * @see Matlab dsp.ChannelSynthesizer
 * @warning This implementation differs from matlab in the decimation parameter. For decim_factor=1, the results should be identical.
 */
class ChannelSynthesizer
{
public:
    ChannelSynthesizer(const ChannelSynthesizer&) = delete;

    //TODO: remove num_taps if filter is calculated
    //TODO: params order (bands, decim, ntaps/coeffs)

    /**
     * @brief Construct ChannelSynthesizer
     * 
     * @param filter Multirate FIR coeffs [num_bands * ntaps]
     * @param num_bands Number of frequency bands
     * @param decim_factor Decimation factor [1 : M-1]
     */
    explicit ChannelSynthesizer(const arr_real& filter, int num_bands, int decim_factor);

    /**
     * @brief Construct ChannelSynthesizer
     * @details Use this function to save memory if multiple Channelizer/ChannelSynthesizer objects can exist at the same time
     * @param filter Pointer to multirate FIR coeffs
     * @param num_bands Number of frequency bands
     * @param decim_factor Decimation factor [1 : M-1]
     */
    explicit ChannelSynthesizer(std::shared_ptr<const arr_real> filter, int num_bands, int decim_factor);

    /**
     * @brief Construct ChannelSynthesizer
     * @details The filter will be calculated using the `design_multirate_fir(1, num_bands, ceil(num_taps / 2.0))`
     * @param num_bands Number of frequency bands
     * @param decim_factor Decimation factor [1 : M-1]
     * @param num_taps Number of filter coefficients per frequency band (expected to be even)
     */
    explicit ChannelSynthesizer(int num_bands, int decim_factor, int num_taps);

    /**
     * @brief Filter bank processing (synthesis)
     * @param x Input subband signal [num_bands]
     * @return arr_cmplx Restored broadband signal [num_bands / decim_factor]
     */
    [[nodiscard]] arr_real process(const arr_cmplx& x);

    arr_real operator()(const arr_cmplx& x) {
        return this->process(x);
    }

    /**
     * @brief Processing frame size
     * @return int frame len
     */
    [[nodiscard]] int frame_len() const noexcept;

private:
    std::shared_ptr<ChannelSynthesizerImpl> d_;
};

}   // namespace dsplib