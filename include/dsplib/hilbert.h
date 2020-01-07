#ifndef HILBERT_H
#define HILBERT_H

#include <dsplib/fir.h>

namespace dsplib {

/*!
 * \brief Hilbert filter class
 */
class hilbert
{
public:
    hilbert();
    explicit hilbert(const arr_real& h);

    //main processing
    arr_cmplx filter(const arr_real& s);

    //current impulse response
    const arr_real& impz() const;

    //calculation of the analytical signal using DFT
    static arr_cmplx process(const arr_real& s);

private:
    fir _fir;       ///< FIR filter for I channel
    arr_real _h;    ///< impulse response
    arr_real _d;    ///< delay for Q channel
};

}   ///< dsplib

#endif // HILBERT_H
