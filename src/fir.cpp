#include <dsplib/fir.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

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
fir::fir(const arr_real &h)
{
    _h = h;
    _d = arr_real::zeros(h.size()-1);
}

//-------------------------------------------------------------------------------------------------
fir_cmplx::fir_cmplx(const arr_cmplx &h)
{
    _h = h;
    _d = arr_cmplx::zeros(h.size()-1);
}

//-------------------------------------------------------------------------------------------------
arr_real fir::process(const arr_real &s)
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
arr_cmplx fir_cmplx::process(const arr_cmplx &s)
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
const arr_real &fir::impz() const
{
    return _h;
}

//-------------------------------------------------------------------------------------------------
const arr_cmplx &fir_cmplx::impz() const
{
    return _h;
}

//-------------------------------------------------------------------------------------------------
template <class T>
static void _conv(const T* x, const T* h, T* r, int nh, int nx)
{
    int nr = nx - nh + 1;
    const T* px = x;
    const T* ph = h;
    T* pr = r;
    for (int i=0; i < nr; ++i)
    {
        pr = r + i;
        px = x + i;
        ph = h + nh - 1;
        *pr = 0;
        for (int k=0; k < nh; ++k)
        {
            *pr += (*px) * (*ph);
            ++px;
            --ph;
        }
    }
}

//-------------------------------------------------------------------------------------------------
arr_real fir::conv(const arr_real &x, const arr_real &h)
{
    arr_real r(x.size() - h.size() + 1);
    _conv <real_t> (x.data(), h.data(), r.data(), h.size(), x.size());
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fir_cmplx::conv(const arr_cmplx &x, const arr_cmplx &h)
{
    arr_cmplx r(x.size() - h.size() + 1);
    _conv <cmplx_t> (x.data(), h.data(), r.data(), h.size(), x.size());
    return r;
}

}   ///< dsplib
