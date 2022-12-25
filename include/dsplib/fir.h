#pragma once

#include <dsplib/array.h>

namespace dsplib {

/*!
 * \brief FIR filter class
 * \todo complex type support
 */
template<typename T>
class FirFilter
{
public:
    explicit FirFilter(const base_array<T>& h)
      : _h(h)
      , _d(h.size() - 1) {
    }

    //main processing
    base_array<T> process(const base_array<T>& s) {
        auto x = _d | s;
        auto r = FirFilter::conv(x, _h);
        int nd = _h.size() - 1;
        int nx = x.size();
        _d = x.slice((nx - nd), nx);
        return r;
    }

    //current impulse response
    [[nodiscard]] const base_array<T>& coeffs() const {
        return _h;
    }

    base_array<T>& coeffs() {
        return _h;
    }

    //convolution operation
    static base_array<T> conv(const base_array<T>& x, const base_array<T>& h);

    base_array<T> operator()(const base_array<T>& x) {
        return this->process(x);
    }

private:
    base_array<T> _h;   ///< impulse response
    base_array<T> _d;   ///< filter delay
};

using fir [[deprecated]] = FirFilter<real_t>;
using fir_cmplx [[deprecated]] = FirFilter<cmplx_t>;

using FirFilterR = FirFilter<real_t>;
using FirFilterC = FirFilter<cmplx_t>;

/*!
 * \brief FFT-based FIR filtering using overlap-add method
 * \details Fast fir implementation for IR len > 200
 */
class FftFilter
{
public:
    FftFilter() = default;
    explicit FftFilter(const arr_real& h);
    explicit FftFilter(const arr_cmplx& h);

    //usually in.size() != out.size()
    arr_real process(const arr_real& x);
    arr_cmplx process(const arr_cmplx& x);

    //optimal input size for y[nx] = process(x[nx])
    [[nodiscard]] int block_size() const {
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

using fir_fft [[deprecated]] = FftFilter;

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
