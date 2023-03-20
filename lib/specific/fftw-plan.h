#pragma once

#include <dsplib/array.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>
#include <dsplib/fft.h>

#include <cstdint>
#include <memory>
#include <cassert>
#include <stdexcept>

#include <fftw3.h>

namespace dsplib {

namespace fftw3 {

#ifdef FFTW_FLOAT
using cmplx_t = fftwf_complex;
using plan_t = fftwf_plan;
constexpr auto& init_plan = fftwf_plan_dft_1d;
constexpr auto& destroy_plan = fftwf_destroy_plan;
constexpr auto& execute = fftwf_execute;
constexpr auto& malloc = fftwf_malloc;
constexpr auto& free = fftwf_free;
#elif FFTW_DOUBLE
using cmplx_t = fftw_complex;
using plan_t = fftw_plan;
constexpr auto& init_plan = fftw_plan_dft_1d;
constexpr auto& destroy_plan = fftw_destroy_plan;
constexpr auto& execute = fftw_execute;
constexpr auto& malloc = fftw_malloc;
constexpr auto& free = fftw_free;
#else
#error "FFTW3 not supported"
#endif

}   // namespace fftw3

class FFTWPlan : public BaseFftPlan
{
public:
    explicit FFTWPlan(uint32_t n)
      : n_{n} {
        static_assert(sizeof(fftw3::cmplx_t) == sizeof(cmplx_t), "complex types is not equal");
        in_ = zeros(n_);
        out_ = zeros(n_);
        auto* in = reinterpret_cast<fftw3::cmplx_t*>(in_.data());
        auto* out = reinterpret_cast<fftw3::cmplx_t*>(out_.data());
        plan_ = fftw3::init_plan(n_, in, out, FFTW_FORWARD, FFTW_MEASURE);
    }

    virtual ~FFTWPlan() {
        fftw3::destroy_plan(plan_);
    }

    arr_cmplx solve(const arr_cmplx& x) const final {
        if (x.size() != n_) {
            throw std::runtime_error("Input size error");
        }
        in_.slice(0, n_) = x;
        fftw3::execute(plan_);
        return out_;
    }

    arr_cmplx solve(const arr_real& x) const final {
        //TODO: use real fft implementation
        return this->solve(arr_cmplx(x));
    }

    virtual int size() const noexcept final {
        return n_;
    }

private:
    mutable fftw3::plan_t plan_;
    uint32_t n_;
    mutable arr_cmplx in_;
    mutable arr_cmplx out_;
};

}   // namespace dsplib