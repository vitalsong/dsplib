#pragma once

#include <dsplib/array.h>
#include <dsplib/keywords.h>

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

    base_array<T> process(const span_t<T>& s) {
        base_array<T> r(s);
        this->process(inplace(r));
        return r;
    }

    void process(inplace_span_t<T> si) {
        auto s = si.get();
        auto x = concatenate(_d, s);
        FirFilter::conv(x, _h);
        s.assign(x.slice(0, s.size()));
        const int nd = _d.size();
        const int nx = x.size();
        _d.slice(0, nd) = x.slice((nx - nd), nx);
    }

    //current impulse response
    [[nodiscard]] const base_array<T>& coeffs() const {
        return _h;
    }

    base_array<T>& coeffs() {
        return _h;
    }

    base_array<T> operator()(const span_t<T>& x) {
        return this->process(x);
    }

private:
    static void conv(mut_span_t<T> x, span_t<T> h);

    base_array<T> _h;   ///< impulse response
    base_array<T> _d;   ///< filter delay
};

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

//Type of linear phase FIR filter
FirType firtype(const dsplib::arr_real& h);

//Window-based FIR filter design (low or high)
//n - filter order
//wn - frequency constraints (0, 1)
//ftype - fir type
//win - custom window (must be n+1 or n+1+rem(n,2) for high/stop)
arr_real fir1(int n, real_t wn, FilterType ftype = FilterType::Low);
arr_real fir1(int n, real_t wn, FilterType ftype, const arr_real& win);

//Window-based FIR filter design (bandpass or stop)
arr_real fir1(int n, real_t wn1, real_t wn2, FilterType ftype = FilterType::Bandpass);
arr_real fir1(int n, real_t wn1, real_t wn2, FilterType ftype, const arr_real& win);

}   // namespace dsplib
