#include <dsplib/fft.h>
#include <dsplib/czt.h>

#include "datacache.h"

#if defined(FFT_BACKEND_FFTW3)
#include "fft2-impl/fftw3.h"
#else
#include "fft2-impl/default.h"
#endif

namespace dsplib {

//-------------------------------------------------------------------------------------------------
//caching czt(n, n, exp(-2pi/n), 1) for nfft != 2^m
//TODO: optional disable caching
using czt_plan_ptr = std::shared_ptr<czt_plan>;
static datacache<size_t, czt_plan_ptr> g_czt_cache;

//-------------------------------------------------------------------------------------------------
//caching fft2
//TODO: optional disable caching
using fft2_plan_ptr = std::shared_ptr<fft2_plan>;
static datacache<size_t, fft2_plan_ptr> g_fft2_cache;

//-------------------------------------------------------------------------------------------------
class fft_plan_impl
{
public:
    explicit fft_plan_impl(int n) {
        const int n2 = 1L << nextpow2(n);
        if (n == n2) {
            //n == 2^K
            if (!g_fft2_cache.cached(n)) {
                auto plan = std::make_shared<fft2_plan>(n);
                g_fft2_cache.update(n, plan);
            }

            auto plan = g_fft2_cache.get(n);
            solve = [plan](const arr_cmplx& x) {
                return plan->solve(x);
            };
        } else {
            //n != 2^K
            if (!g_czt_cache.cached(n)) {
                cmplx_t w = expj(-2 * pi / n);
                auto plan = std::make_shared<czt_plan>(n, n, w);
                g_czt_cache.update(n, plan);
            }

            auto plan = g_czt_cache.get(n);
            solve = [plan](const arr_cmplx& x) {
                return plan->solve(x);
            };
        }
    }

    std::function<arr_cmplx(const arr_cmplx&)> solve;
};

//-------------------------------------------------------------------------------------------------
fft_plan::fft_plan(int n)
  : _d{std::make_shared<fft_plan_impl>(n)} {
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fft_plan::operator()(const arr_cmplx& x) const {
    return _d->solve(x);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fft_plan::solve(const arr_cmplx& x) const {
    return _d->solve(x);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fft(const arr_cmplx& arr) {
    fft_plan plan(arr.size());
    return plan(arr);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fft(const arr_cmplx& x, int n) {
    if (n == x.size()) {
        return fft(x);
    }

    if (n > x.size()) {
        return fft(zeropad(x, n));
    }

    return fft(x.slice(0, n));
}

}   // namespace dsplib
