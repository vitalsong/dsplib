#include "specific/fft.h"

#include <dsplib/czt.h>
#include <dsplib/math.h>

#include <NE10_dsp.h>

namespace dsplib::specific {

namespace {

//init library
static const ne10_result_t _init = ne10_init_dsp(NE10_OK);

//x[0:N/2+1] -> x[0:N]
void _rfft_to_fft(arr_cmplx& x) {
    const int n = x.size();
    for (int i = 1; i < n / 2; ++i) {
        x[n - i] = conj(x[i]);
    }
}

//-------------------------------------------------------------------------------------------------
class Ne10PlanCmplx : public BaseFftPlanC
{
public:
    explicit Ne10PlanCmplx(int n)
      : _n{n}
      , _in(n)
      , _out(n) {
        _plan = ne10_fft_alloc_c2c_float32(_n);
    }

    ~Ne10PlanCmplx() override {
        ne10_fft_destroy_c2c_float32(_plan);
    }

    [[nodiscard]] dsplib::arr_cmplx solve(const dsplib::arr_cmplx& in) const final {
        DSPLIB_ASSERT(in.size() == _n, "Input vector size is not equal FFT");
        _in.slice(0, _n) = in.slice(0, _n);
        auto* pin = reinterpret_cast<ne10_fft_cpx_float32_t*>(_in.data());
        auto* pout = reinterpret_cast<ne10_fft_cpx_float32_t*>(_out.data());
        ne10_fft_c2c_1d_float32(pout, pin, _plan, 0);
        return _out;
    }

    [[nodiscard]] int size() const noexcept final {
        return _n;
    }

private:
    int _n;
    mutable ne10_fft_cfg_float32_t _plan{nullptr};
    mutable dsplib::arr_cmplx _in;
    mutable dsplib::arr_cmplx _out;
};

//-------------------------------------------------------------------------------------------------
class Ne10PlanReal : public BaseFftPlanR
{
public:
    explicit Ne10PlanReal(int n)
      : _n{n}
      , _in(n)
      , _out(n) {
        _plan = ne10_fft_alloc_r2c_float32(_n);
    }

    ~Ne10PlanReal() override {
        ne10_fft_destroy_r2c_float32(_plan);
    }

    [[nodiscard]] dsplib::arr_cmplx solve(const dsplib::arr_real& in) const final {
        DSPLIB_ASSERT(in.size() == _n, "Input vector size is not equal FFT");
        _in.slice(0, _n) = in.slice(0, _n);
        auto* pin = reinterpret_cast<ne10_float32_t*>(_in.data());
        auto* pout = reinterpret_cast<ne10_fft_cpx_float32_t*>(_out.data());
        ne10_fft_r2c_1d_float32(pout, pin, _plan);
        _rfft_to_fft(_out);
        return _out;
    }

    [[nodiscard]] int size() const noexcept final {
        return _n;
    }

private:
    int _n;
    mutable ne10_fft_r2c_cfg_float32_t _plan{nullptr};
    mutable dsplib::arr_real _in;
    mutable dsplib::arr_cmplx _out;
};

//-------------------------------------------------------------------------------------------------
class CztPlanR : public BaseFftPlanR
{
public:
    explicit CztPlanR(int n)
      : _plan(n) {
    }

    [[nodiscard]] arr_cmplx solve(const arr_real& x) const final {
        return _plan.solve(arr_cmplx(x));
    }

    [[nodiscard]] int size() const noexcept final {
        return _plan.size();
    }

private:
    FftPlan _plan;
};

}   // namespace

//-------------------------------------------------------------------------------------------------
std::shared_ptr<BaseFftPlanC> get_fft_plan(int n) {
    //TODO: check factorization and use czt
    if (!ispow2(n)) {
        const cmplx_t w = expj(-2 * pi / n);
        return std::make_shared<CztPlan>(n, n, w);
    }
    return std::make_shared<Ne10PlanCmplx>(n);
}

std::shared_ptr<BaseFftPlanR> get_rfft_plan(int n) {
    //TODO: check factorization and use czt
    if (!ispow2(n)) {
        return std::make_shared<CztPlanR>(n);
    }
    return std::make_shared<Ne10PlanReal>(n);
}

}   // namespace dsplib::specific