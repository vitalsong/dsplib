#pragma once

#include <dsplib/math.h>
#include <dsplib/fft.h>

#include <memory>

namespace dsplib {

class PlanTree;

//The Cooley-Tukey FFT algorithm for general factorizations
class FactorFFTPlan : public FftPlanC
{
public:
    explicit FactorFFTPlan(int n);
    ~FactorFFTPlan() override = default;

    [[nodiscard]] arr_cmplx solve(span_t<cmplx_t> x) const final;
    void solve(span_t<cmplx_t> x, mut_span_t<cmplx_t> r) const final;
    [[nodiscard]] int size() const noexcept final;

private:
    const int _n;
    const arr_cmplx _twiddle;
    std::shared_ptr<PlanTree> _plan;
};

class FactorFFTPlanR : public FftPlanR
{
public:
    explicit FactorFFTPlanR(int n)
      : _plan{n} {
    }

    ~FactorFFTPlanR() override = default;

    [[nodiscard]] arr_cmplx solve(span_t<real_t> x) const final {
        //TODO: real optimization (for odd sizes)
        return _plan.solve(complex(x));
    }

    void solve(span_t<real_t> x, mut_span_t<cmplx_t> r) const final {
        _plan.solve(complex(x), r);
    }

    [[nodiscard]] int size() const noexcept final {
        return _plan.size();
    }

private:
    FactorFFTPlan _plan;
};

}   // namespace dsplib