#pragma once

#include <dsplib/math.h>
#include <dsplib/utils.h>

#include <NE10.h>
#include <cassert>

namespace dsplib {

using ne10_cmplx_t = ne10_fft_cpx_float32_t;

//-------------------------------------------------------------------------------------------------
class fft2_plan
{
public:
    explicit fft2_plan(int n) {
        static_assert(sizeof(ne10_cmplx_t) == sizeof(dsplib::cmplx_t), "complex types is not equal");
        const int n2 = 1L << nextpow2(n);
        assert(n2 == n);
        _n = n;
        _plan = ne10_fft_alloc_c2c_float32(_n);
    }

    ~fft2_plan() {
        ne10_fft_destroy_c2c_float32(_plan);
    }

    arr_cmplx solve(const arr_cmplx& x) {
        assert(x.size() == _n);
        arr_cmplx in(x);
        arr_cmplx out(_n);
        ne10_fft_c2c_1d_float32_neon((ne10_cmplx_t*)out.data(), (ne10_cmplx_t*)in.data(), _plan, 0);
        return out;
    }

private:
    int _n;
    ne10_fft_cfg_float32_t _plan;
};

}   // namespace dsplib