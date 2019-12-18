#ifndef FIR_H
#define FIR_H

#include <dsplib/array.h>

namespace dsplib {

/*!
 * \brief FIR filter class
 */
class fir
{
public:
    fir();
    explicit fir(const arr_real& h);

    //main processing
    arr_real process(const arr_real& s);

    //current impulse response
    const arr_real& impz() const;

    //convolution operation
    static arr_real conv(const arr_real& x, const arr_real& h);

private:
    arr_real _h;     ///< impulse response
    arr_real _d;     ///< filter delay
};

} ///< dsplib

#endif // FIR_H
