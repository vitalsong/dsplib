#pragma once

#include <dsplib/delay.h>
#include <dsplib/fir.h>

namespace dsplib {

// calculation of the analytical signal using DFT
arr_cmplx hilbert(const arr_real& x);

// uses an n-point FFT
arr_cmplx hilbert(const arr_real& x, int n);

/*!
 * \brief Hilbert filter (FIR filter based)
 * \details Algorithm: out = delay(in, M/2) + j * fir(in)
 * \todo Use complex FFT filter
 */
class HilbertFilter
{
public:
    //default bandpass filter IR
    //flen - FIR filter length
    //tw - transition bandwidth
    explicit HilbertFilter(int flen = 51, real_t tw = 0.01);

    explicit HilbertFilter(const arr_real& h);

    //main processing
    arr_cmplx process(const arr_real& s);

    //impulse response
    [[nodiscard]] const arr_real& impz() const;

    arr_cmplx operator()(const arr_real& x) {
        return this->process(x);
    }

    // Design complex Hilbert FIR (Window Method)
    // see: https://www.dsprelated.com/freebooks/sasp/Filtering_Windowing_Ideal_Hilbert_Transform.html
    // flen - FIR filter length
    // fs - sample rate (Hz)
    // f1 - lower pass-band limit = transition bandwidth (Hz)
    static arr_cmplx design_fir(int flen, real_t fs, real_t f1);

private:
    FirFilter<real_t> _fir;   ///< FIR filter for I channel
    DelayReal _d;             ///< delay for Q channel
};

}   // namespace dsplib
