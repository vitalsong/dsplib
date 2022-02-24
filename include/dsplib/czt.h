#pragma once

#include <dsplib/array.h>

namespace dsplib {

class czt_plan
{
public:
    explicit czt_plan(int n, int m, cmplx_t w = cmplx_t{0.7071, 0.7071}, cmplx_t a = 1);
    arr_cmplx operator()(const arr_cmplx& x) const;

private:
    int _n;
    int _m;
    cmplx_t _w;
    cmplx_t _a;
    arr_cmplx _ich;
    arr_cmplx _cp;
    arr_cmplx _rp;
};

/*!
* \brief One-shot Chirp Z-transform
* \warning May be slow when called periodically (not caching)
* \param m Transform length
* \param w Ratio between spiral contour points
* \param a Spiral contour initial point
* \return Chirp Z-transform
*/
arr_cmplx czt(arr_cmplx x, int m, cmplx_t w = cmplx_t{0.7071, 0.7071}, cmplx_t a = 1);

}   // namespace dsplib
