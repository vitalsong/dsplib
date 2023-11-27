#include <dsplib/fft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

#include "lru-cache.h"
#include "specific/fft.h"

#include <cassert>

namespace dsplib {

namespace {
constexpr int FFT_CACHE_SIZE = DSPLIB_FFT_CACHE_SIZE;
}

//-------------------------------------------------------------------------------------------------
FftPlan::FftPlan(int n) {
    thread_local LRUCache<int, std::shared_ptr<BaseFftPlanC>> cache{FFT_CACHE_SIZE};
    if (!cache.exists(n)) {
        cache.put(n, specific::get_fft_plan(n));
    }
    _d = cache.get(n);
}

FftPlanR::FftPlanR(int n) {
    thread_local LRUCache<int, std::shared_ptr<BaseFftPlanR>> cache{FFT_CACHE_SIZE};
    if (!cache.exists(n)) {
        cache.put(n, specific::get_rfft_plan(n));
    }
    _d = cache.get(n);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fft(const arr_cmplx& x) {
    auto plan = FftPlan(x.size());
    return plan(x);
}

arr_cmplx fft(const arr_cmplx& x, int n) {
    if (n == x.size()) {
        return fft(x);
    }

    if (n > x.size()) {
        return fft(zeropad(x, n));
    }

    return fft(x.slice(0, n));
}

arr_cmplx fft(const arr_real& x) {
    auto plan = FftPlanR(x.size());
    return plan(x);
}

arr_cmplx fft(const arr_real& x, int n) {
    if (n == x.size()) {
        return fft(x);
    }

    if (n > x.size()) {
        return fft(zeropad(x, n));
    }

    return fft(x.slice(0, n));
}

}   // namespace dsplib
