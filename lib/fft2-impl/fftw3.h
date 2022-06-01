#pragma once

#include <dsplib/math.h>
#include <dsplib/utils.h>

#include <fftw3.h>
#include <cassert>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
class fft2_plan
{
public:
    explicit fft2_plan(int n) {
        static_assert(sizeof(fftw_complex) == sizeof(dsplib::cmplx_t), "complex types is not equal");
        const int n2 = 1L << nextpow2(n);
        assert(n2 == n);
        _n = n;
        _in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * n);
        _out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * n);
        _plan = fftw_plan_dft_1d(n, _in, _out, FFTW_FORWARD, FFTW_ESTIMATE);
    }

    ~fft2_plan() {
        fftw_destroy_plan(_plan);
        fftw_free(_in);
        fftw_free(_out);
    }

    arr_cmplx solve(const arr_cmplx& x) {
        assert(x.size() == _n);
        memcpy(_in, x.data(), _n * sizeof(fftw_complex));
        fftw_execute(_plan);
        arr_cmplx r(_n);
        memcpy(r.data(), _out, _n * sizeof(fftw_complex));
        return r;
    }

private:
    int _n;
    fftw_plan _plan;
    fftw_complex* _in;
    fftw_complex* _out;
};

}   // namespace dsplib
