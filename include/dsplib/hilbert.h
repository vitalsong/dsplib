#pragma once

#include <dsplib/fir.h>

namespace dsplib {

/*!
 * \brief Calculation of the analytical signal using DFT
 * \param s Input [n]
 * \return Output [n]
 */
arr_cmplx hilbert(const arr_real& s);

/*!
 * \brief Hilbert filter class
 */
class HilbertFilter
{
public:
    //default bandpass filter IR
    //TODO: generate optimal IR
    HilbertFilter();

    explicit HilbertFilter(const arr_real& h);

    //main processing
    arr_cmplx process(const arr_real& s);

    //current impulse response
    [[nodiscard]] const arr_real& impz() const;

    arr_cmplx operator()(const arr_real& x) {
        return this->process(x);
    }

private:
    FirFilter<real_t> _fir;   ///< FIR filter for I channel
    arr_real _d;              ///< delay for Q channel
};

using hilbert_filter [[deprecated]] = HilbertFilter;

}   // namespace dsplib
