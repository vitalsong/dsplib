#pragma once

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
    arr_real _h;   ///< impulse response
    arr_real _d;   ///< filter delay
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
    arr_cmplx process(const arr_cmplx& s);

    //current impulse response
    const arr_cmplx& impz() const;

    //convolution operation
    static arr_cmplx conv(const arr_cmplx& x, const arr_cmplx& h);

private:
    arr_cmplx _h;   ///< impulse response
    arr_cmplx _d;   ///< filter delay
};

/*!
 * \brief Fast fir implementation for IR len > 200
 */
class fir_fft
{
public:
    fir_fft() = default;
    explicit fir_fft(const arr_real& h);
    explicit fir_fft(const arr_cmplx& h);

    //usually in.size() != out.size()
    arr_real process(const arr_real& x);
    arr_cmplx process(const arr_cmplx& x);

    //optimal input size for y[nx] = process(x[nx])
    int block_size() const
    {
        return _n;
    }

private:
    dsplib::arr_cmplx _x;
    dsplib::arr_cmplx _h;
    dsplib::arr_cmplx _olap;
    int _nx{0};
    int _m{0};
    int _n{0};
};

}   // namespace dsplib
