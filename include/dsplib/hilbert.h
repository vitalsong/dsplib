#ifndef HILBERT_H
#define HILBERT_H

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
class hilbert_filter
{
public:
    hilbert_filter();
    explicit hilbert_filter(const arr_real& h);

    //main processing
    arr_cmplx process(const arr_real& s);

    //current impulse response
    const arr_real& impz() const;

private:
    fir _fir;       ///< FIR filter for I channel
    arr_real _h;    ///< impulse response
    arr_real _d;    ///< delay for Q channel
};

}   ///< dsplib

#endif // HILBERT_H
