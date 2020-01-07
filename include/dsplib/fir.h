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
    arr_real filter(const arr_real& s);

    //current impulse response
    const arr_real& impz() const;

    //convolution operation
    static arr_real conv(const arr_real& x, const arr_real& h);

private:
    arr_real _h;     ///< impulse response
    arr_real _d;     ///< filter delay
};

/*!
 * \brief FIR complex filter class
 */
class fir_cmplx
{
public:
    fir_cmplx();
    explicit fir_cmplx(const arr_cmplx& h);

    //main processing
    arr_cmplx filter(const arr_cmplx& s);

    //current impulse response
    const arr_cmplx& impz() const;

    //convolution operation
    static arr_cmplx conv(const arr_cmplx& x, const arr_cmplx& h);

private:
    arr_cmplx _h;     ///< impulse response
    arr_cmplx _d;     ///< filter delay
};

} ///< dsplib

#endif // FIR_H
