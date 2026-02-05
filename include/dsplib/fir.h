#pragma once

#include <dsplib/array.h>
#include <dsplib/keywords.h>
#include <dsplib/math.h>
#include <dsplib/fft.h>
#include <dsplib/ifft.h>

namespace dsplib {

/*!
 * \brief FIR filter class
 * \todo complex type support
 */
template<typename T>
class FirFilter
{
public:
    explicit FirFilter(span_t<T> h)
      : _h(h)
      , _d(h.size() - 1) {
    }

    base_array<T> process(span_t<T> x) {
        base_array<T> r(x);
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
    [[nodiscard]] span_t<T> coeffs() const {
        return make_span(_h);
    }

    mut_span_t<T> coeffs() {
        return make_span(_h);
    }

    base_array<T> operator()(span_t<T> x) {
        return this->process(x);
    }

private:
    static void conv(mut_span_t<T> x, span_t<T> h);

    base_array<T> _h;   ///< impulse response
    base_array<T> _d;   ///< filter delay
};

using FirFilterR = FirFilter<real_t>;
using FirFilterC = FirFilter<cmplx_t>;

template<typename U>
FirFilter(const base_array<U>&) -> FirFilter<U>;

template<typename T>
class FftFilterImpl;

/*!
 * \brief FFT-based FIR filtering using overlap-add method
 * \details Fast fir implementation for large IR length (usually > 200)
 */
template<typename T>
class FftFilter
{
public:
    explicit FftFilter(span_t<T> h);

    base_array<T> process(span_t<T> x);

    base_array<T> operator()(span_t<T> x) {
        return this->process(x);
    }

    [[nodiscard]] int block_size() const;

private:
    std::shared_ptr<FftFilterImpl<T>> d_;
};

using FftFilterR = FftFilter<real_t>;
using FftFilterC = FftFilter<cmplx_t>;

template<typename U>
FftFilter(const base_array<U>&) -> FftFilter<U>;

//Type of linear phase FIR filter
FirType firtype(span_real h);

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
