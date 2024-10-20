#pragma once

#include <dsplib/math.h>
#include <dsplib/fft.h>

#include <memory>

namespace dsplib {

class PlanTree;

//The Cooley-Tukey FFT algorithm for general factorizations
class FactorFFTPlan : public BaseFftPlanC
{
public:
    explicit FactorFFTPlan(int n);
    ~FactorFFTPlan() = default;
    [[nodiscard]] arr_cmplx solve(const arr_cmplx& x) const final;
    [[nodiscard]] int size() const noexcept final;

private:
    int _n;
    arr_cmplx _twiddle;
    mutable arr_cmplx _px;   ///< tmp matrix for transpose
    std::shared_ptr<PlanTree> _plan;
};

class FactorFFTPlanR : public BaseFftPlanR
{
public:
    explicit FactorFFTPlanR(int n)
      : _plan{n} {
    }

    ~FactorFFTPlanR() = default;

    [[nodiscard]] arr_cmplx solve(const arr_real& x) const final {
        //TODO: real optimization (for odd sizes)
        return _plan.solve(complex(x));
    }

    [[nodiscard]] int size() const noexcept final {
        return _plan.size();
    }

private:
    FactorFFTPlan _plan;
};

}   // namespace dsplib