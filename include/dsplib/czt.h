#pragma once

#include <dsplib/fft.h>
#include <memory>

namespace dsplib {

class CztPlanImpl;

class CztPlan : public BaseFftPlanC
{
public:
    explicit CztPlan(int n, int m, cmplx_t w, cmplx_t a = 1);
    [[nodiscard]] arr_cmplx solve(const arr_cmplx& x) const final;
    [[nodiscard]] int size() const noexcept final;

    arr_cmplx operator()(const arr_cmplx& x) const {
        return this->solve(x);
    }

private:
    std::shared_ptr<CztPlanImpl> _d;
};

/*!
* \brief One-shot Chirp Z-transform
* \warning May be slow when called periodically (not caching)
* \param m Transform length
* \param w Ratio between spiral contour points
* \param a Spiral contour initial point
* \return Chirp Z-transform
*/
arr_cmplx czt(const arr_cmplx& x, int m, cmplx_t w, cmplx_t a = 1);

}   // namespace dsplib
