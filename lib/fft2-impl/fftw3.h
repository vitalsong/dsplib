#pragma once

#include <dsplib/math.h>
#include <dsplib/utils.h>

#include <fftw3.h>
#include <cassert>

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

namespace dsplib {

//-------------------------------------------------------------------------------------------------
class fft2_plan
{
public:
    explicit fft2_plan(int n) {
        static_assert(sizeof(fftw3::cmplx_t) == sizeof(dsplib::cmplx_t), "complex types is not equal");
        const int n2 = (1L << nextpow2(n));
        assert(n2 == n);
        _n = n;
        _in = (fftw3::cmplx_t*)fftw3::malloc(sizeof(fftw3::cmplx_t) * n);
        _out = (fftw3::cmplx_t*)fftw3::malloc(sizeof(fftw3::cmplx_t) * n);
        _plan = fftw3::init_plan(n, _in, _out, FFTW_FORWARD, FFTW_ESTIMATE);
    }

    ~fft2_plan() {
        fftw3::destroy_plan(_plan);
        fftw3::free(_in);
        fftw3::free(_out);
    }

    arr_cmplx solve(const arr_cmplx& x) {
        assert(x.size() == _n);
        memcpy(_in, x.data(), _n * sizeof(fftw3::cmplx_t));
        fftw3::execute(_plan);
        arr_cmplx r(_n);
        memcpy(r.data(), _out, _n * sizeof(fftw3::cmplx_t));
        return r;
    }

private:
    int _n;
    fftw3::plan_t _plan;
    fftw3::cmplx_t* _in;
    fftw3::cmplx_t* _out;
};

}   // namespace dsplib
