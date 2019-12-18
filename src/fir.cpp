#include <dsplib/fir.h>
#include <dsplib/math.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
fir::fir()
{
    _h = arr_real(1);
    _h[0] = 1;
}

//-------------------------------------------------------------------------------------------------
fir::fir(const arr_real &h)
{
    _h = h;
    _d = arr_real(h.size()-1);
    zeroing(_d);
}

//-------------------------------------------------------------------------------------------------
arr_real fir::process(const arr_real &s)
{
    auto x = arr_real::join(_d, s);

    //convolution
    arr_real r = fir::conv(x, _h);

    //keep filter delay
    int nd = _h.size() - 1;
    int nx = x.size();
    _d = arr_real::slice(x, (nx - nd), (nx - 1));

    return r;
}

//-------------------------------------------------------------------------------------------------
const arr_real &fir::impz() const
{
    return _h;
}

//-------------------------------------------------------------------------------------------------
arr_real fir::conv(const arr_real &x, const arr_real &h)
{
    int nr = x.size() - h.size() + 1;
    int nh = h.size();

    arr_real r(nr);
    for (int i=0; i < nr; ++i)
    {
        r[i] = 0;
        for (int k=0; k < nh; ++k) {
            r[i] += x[i+k] * h[nh-1-k];
        }
    }

    return r;
}

}   ///< dsplib
