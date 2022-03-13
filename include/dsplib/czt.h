#pragma once

#include <dsplib/array.h>
#include <memory>

namespace dsplib {

class czt_plan_impl;

class czt_plan
{
public:
    explicit czt_plan(int n, int m, cmplx_t w, cmplx_t a = 1);
    arr_cmplx operator()(const arr_cmplx& x) const;
    arr_cmplx solve(const arr_cmplx& x) const;

private:
    std::shared_ptr<czt_plan_impl> _d;
};

/*!
* \brief One-shot Chirp Z-transform
* \warning May be slow when called periodically (not caching)
* \param m Transform length
* \param w Ratio between spiral contour points
* \param a Spiral contour initial point
* \return Chirp Z-transform
*/
arr_cmplx czt(arr_cmplx x, int m, cmplx_t w, cmplx_t a = 1);

}   // namespace dsplib
