#pragma once

#include <dsplib/array.h>

namespace dsplib {

/*!
 * \brief FIR filter class
 * \todo complex type support
 */
class fir
{
public:
    fir();
    explicit fir(const arr_real& h);

    //main processing
    arr_real process(const arr_real& s);

    //current impulse response
    const arr_real& coeffs() const {
        return _h;
    }

    arr_real& coeffs() {
        return _h;
    }

    //convolution operation
    static arr_real conv(const arr_real& x, const arr_real& h);

    arr_real operator()(const arr_real& x) {
        return this->process(x);
    }

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
    const arr_cmplx& coeffs() const {
        return _h;
    }

    arr_cmplx& coeffs() {
        return _h;
    }

    //convolution operation
    static arr_cmplx conv(const arr_cmplx& x, const arr_cmplx& h);

    arr_cmplx operator()(const arr_cmplx& x) {
        return this->process(x);
    }

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
    int block_size() const {
        return _n;
    }

    arr_real operator()(const arr_real& x) {
        return this->process(x);
    }

    arr_cmplx operator()(const arr_cmplx& x) {
        return this->process(x);
    }

private:
    dsplib::arr_cmplx _x;
    dsplib::arr_cmplx _h;
    dsplib::arr_cmplx _olap;
    int _nx{0};
    int _m{0};
    int _n{0};
};

enum class FilterType
{
    Low,
    High,
    Bandpass,
    Bandstop
};

enum class FirType : int
{
    NonlinearPhase = 0,   //Nonlinear phase
    EvenSymm = 1,         //Even-order symmetric coefficients
    OddSym = 2,           //Odd-order symmetric coefficients
    EvenAntiSym = 3,      //Even-order antisymmetric coefficients
    OddAntiSym = 4        //Odd-order antisymmetric coefficients
};

//Type of linear phase FIR filter
FirType firtype(const dsplib::arr_real& h);

//Window-based FIR filter design (low or high)
//n - filter order
//wn - frequency constraints (0, 1)
//ftype - fir type
arr_real fir1(int n, real_t wn, FilterType ftype = FilterType::Low);

//Window-based FIR filter design (bandpass or stop)
arr_real fir1(int n, real_t wn1, real_t wn2, FilterType ftype = FilterType::Bandpass);

}   // namespace dsplib
