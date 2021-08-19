#include <dsplib/fir.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>
#include <dsplib/fft.h>
#include <dsplib/ifft.h>
#include <cassert>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
fir::fir()
{
    _h = arr_real(1);
    _h[0] = 1;
}

//-------------------------------------------------------------------------------------------------
fir_cmplx::fir_cmplx()
{
    _h = arr_cmplx(1);
    _h[0] = 1;
}

//-------------------------------------------------------------------------------------------------
fir::fir(const arr_real& h)
{
    _h = h;
    _d = zeros(h.size() - 1);
}

//-------------------------------------------------------------------------------------------------
fir_cmplx::fir_cmplx(const arr_cmplx& h)
{
    _h = h;
    _d = arr_cmplx(zeros(h.size() - 1));
}

//-------------------------------------------------------------------------------------------------
arr_real fir::process(const arr_real& s)
{
    auto x = concatenate(_d, s);

    //convolution
    arr_real r = fir::conv(x, _h);

    //keep filter delay
    int nd = _h.size() - 1;
    int nx = x.size();
    _d = x.slice((nx - nd), nx);

    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fir_cmplx::process(const arr_cmplx& s)
{
    auto x = concatenate(_d, s);

    //convolution
    arr_cmplx r = fir_cmplx::conv(x, _h);

    //keep filter delay
    int nd = _h.size() - 1;
    int nx = x.size();
    _d = x.slice((nx - nd), nx);

    return r;
}

//-------------------------------------------------------------------------------------------------
const arr_real& fir::impz() const
{
    return _h;
}

//-------------------------------------------------------------------------------------------------
const arr_cmplx& fir_cmplx::impz() const
{
    return _h;
}

//-------------------------------------------------------------------------------------------------
static void _conv(const real_t* restrict x, const real_t* restrict h, real_t* restrict r, int nh, int nx)
{
    const int nr = nx - nh + 1;
    assert(nr > 0);
    for (int i = 0; i < nr; ++i) {
        r[i] = 0;
        for (int k = 0; k < nh; ++k) {
            r[i] += x[i + k] * h[k];
        }
    }
}

//-------------------------------------------------------------------------------------------------
static void _conv(const cmplx_t* restrict x, const cmplx_t* restrict h, cmplx_t* restrict r, int nh, int nx)
{
    const int nr = nx - nh + 1;
    assert(nr > 0);
    for (int i = 0; i < nr; ++i) {
        r[i] = 0;
        for (int k = 0; k < nh; ++k) {
            r[i] += x[i + k] * h[k].conj();
        }
    }
}

//-------------------------------------------------------------------------------------------------
arr_real fir::conv(const arr_real& x, const arr_real& h)
{
    arr_real r(x.size() - h.size() + 1);
    auto hh = flip(h);
    _conv(x.data(), hh.data(), r.data(), hh.size(), x.size());
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fir_cmplx::conv(const arr_cmplx& x, const arr_cmplx& h)
{
    arr_cmplx r(x.size() - h.size() + 1);
    auto hh = flip(h);
    _conv(x.data(), hh.data(), r.data(), hh.size(), x.size());
    return r;
}

//-------------------------------------------------------------------------------------------------
fir_fft::fir_fft(const arr_cmplx& h)
{
    int fft_len = 1L << nextpow2(2 * h.size());
    _n = fft_len - h.size();
    _m = h.size();
    _olap = zeros(_m);
    auto dh = concatenate(conj(h), arr_cmplx(fft_len - h.size()));
    _h = fft(dh);
    _x = zeros(fft_len);
}

//-------------------------------------------------------------------------------------------------
fir_fft::fir_fft(const arr_real& h)
  : fir_fft(arr_cmplx(h))
{}

//-------------------------------------------------------------------------------------------------
arr_cmplx fir_fft::process(const arr_cmplx& x)
{
    if (_n == 0) {
        return x;
    }

    const int nr = (x.size() + _nx) / _n * _n;
    arr_cmplx r(nr);
    cmplx_t* pr = r.data();
    for (const auto& val : x) {
        _x[_nx] = val;
        _nx += 1;
        if (_nx == _n) {
            const auto yy = fft(_x) * _h;
            auto ry = ifft(yy);
            for (size_t i = 0; i < _m; i++) {
                ry[i] += _olap[i];
            }
            for (size_t i = 0; i < _m; i++) {
                _olap[i] = ry[i + _n];
            }
            for (size_t i = 0; i < _n; i++) {
                pr[i] = ry[i];
            }
            pr += _n;
            _nx = 0;
        }
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real fir_fft::process(const arr_real& x)
{
    return real(process(arr_cmplx(x)));
}

}   // namespace dsplib
