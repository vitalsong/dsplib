#include "fft/factory.h"
#include "fft/cmplx-ifft.h"
#include "fft/fact-fft.h"
#include "fft/primes-fft.h"
#include "fft/real-fft.h"
#include "fft/real-ifft.h"

#include <NE10_dsp.h>
#include <NE10_init.h>

//init library
static const ne10_result_t _init = ne10_init_dsp(NE10_OK);

namespace dsplib::internal {

static_assert(sizeof(real_t) == sizeof(float), "real_t type must be float32");

namespace {

//------------------------------------------------------------------------------------------------------------------------
//complex FFT
class NE10PlanC : public FftPlanC
{
public:
    explicit NE10PlanC(int n)
      : _n{n}
      , _in(n) {
        _plan = ne10_fft_alloc_c2c_float32(_n);
    }

    virtual ~NE10PlanC() {
        ne10_fft_destroy_c2c_float32(_plan);
    }

    arr_cmplx solve(span_t<cmplx_t> x) const final {
        arr_cmplx r(x.size());
        this->solve(x, r);
        return r;
    }

    void solve(span_t<cmplx_t> x, mut_span_t<cmplx_t> r) const final {
        DSPLIB_ASSERT(x.size() == _n, "Input vector size is not equal FFT");
        DSPLIB_ASSERT(r.size() == _n, "Output vector size is not equal FFT");
        _in.slice(0, _n) = x;
        auto* pin = reinterpret_cast<ne10_fft_cpx_float32_t*>(_in.data());
        auto* pout = reinterpret_cast<ne10_fft_cpx_float32_t*>(r.data());
        ne10_fft_c2c_1d_float32(pout, pin, _plan, 0);
    }

    int size() const noexcept final {
        return _n;
    }

private:
    int _n;
    mutable ne10_fft_cfg_float32_t _plan{nullptr};
    mutable arr_cmplx _in;
};

//------------------------------------------------------------------------------------------------------------------------
//real FFT
class NE10PlanR : public FftPlanR
{
public:
    explicit NE10PlanR(int n)
      : _n{n}
      , _in(n) {
        _plan = ne10_fft_alloc_r2c_float32(_n);
    }

    virtual ~NE10PlanR() {
        ne10_fft_destroy_r2c_float32(_plan);
    }

    arr_cmplx solve(span_t<real_t> x) const final {
        arr_cmplx r(x.size());
        this->solve(x, r);
        return r;
    }

    void solve(span_t<real_t> x, mut_span_t<cmplx_t> r) const final {
        DSPLIB_ASSERT(x.size() == _n, "Input vector size is not equal FFT");
        DSPLIB_ASSERT(r.size() == _n, "Output vector size is not equal FFT");
        _in.slice(0, _n) = x;
        auto* pin = reinterpret_cast<ne10_float32_t*>(_in.data());
        auto* pout = reinterpret_cast<ne10_fft_cpx_float32_t*>(r.data());
        ne10_fft_r2c_1d_float32(pout, pin, _plan);
        const int n2 = (_n % 2 == 0) ? (_n / 2) : (_n / 2 + 1);
        for (size_t i = 1; i < n2; ++i) {
            r[_n - i] = r[i].conj();
        }
    }

    int size() const noexcept final {
        return _n;
    }

private:
    int _n;
    mutable ne10_fft_r2c_cfg_float32_t _plan{nullptr};
    mutable arr_real _in;
};

}   // namespace

std::shared_ptr<FftPlanC> get_fft_plan(int n) {
    if (ispow2(n)) {
        return std::make_shared<NE10PlanC>(n);
    }
    if (isprime(n)) {
        return std::make_shared<PrimesFftC>(n);
    }
    return std::make_shared<FactorFFTPlan>(n);
}

std::shared_ptr<FftPlanR> get_rfft_plan(int n) {
    if (ispow2(n)) {
        return std::make_shared<NE10PlanR>(n);
    }
    if (n % 2 == 0) {
        return std::make_shared<RealFftPlan>(n);
    }
    if (isprime(n)) {
        return std::make_shared<PrimesFftR>(n);
    }
    return std::make_shared<FactorFFTPlanR>(n);
}

std::shared_ptr<IfftPlanC> get_ifft_plan(int n) {
    return std::make_shared<CmplxIfftPlan>(n);
}

std::shared_ptr<IfftPlanR> get_irfft_plan(int n) {
    return std::make_shared<RealIfftPlan>(n);
}

}   // namespace dsplib::internal