#pragma once

#include <dsplib/fft.h>
#include <memory>

namespace dsplib {

class CztPlanImpl;

class CztPlan : public BaseFftPlan
{
public:
    explicit CztPlan(int n, int m, cmplx_t w, cmplx_t a = 1);
    [[nodiscard]] arr_cmplx solve(const arr_cmplx& x) const final;
    [[nodiscard]] arr_cmplx solve(const arr_real& x) const final;
    [[nodiscard]] int size() const noexcept final;

    arr_cmplx operator()(const arr_cmplx& x) const {
        return this->solve(x);
    }

    arr_cmplx operator()(const arr_real& x) const {
        return this->solve(arr_cmplx(x));
    }

private:
    std::shared_ptr<CztPlanImpl> _d;
};

using czt_plan [[deprecated]] = CztPlan;

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
