#include <dsplib/fft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

#include "fft/fact-fft.h"
#include "fft/primes-fft.h"
#include "fft/factory.h"
#include "fft/pow2-fft.h"
#include "fft/real-fft.h"
#include "fft/small-fft.h"
#include "lru-cache.h"

#include <cassert>
#include <memory>

namespace dsplib {

namespace {

constexpr int FFT_CACHE_SIZE = DSPLIB_FFT_CACHE_SIZE;

static_assert(FFT_CACHE_SIZE > 0);

std::shared_ptr<BaseFftPlanC> _get_fft_plan(int n) {
    if (isprime(n)) {
        return std::make_shared<PrimesFftC>(n);
    }
    if (ispow2(n)) {
        return std::make_shared<Pow2FftPlan>(n);
    }
    return std::make_shared<FactorFFTPlan>(n);
}

std::shared_ptr<BaseFftPlanR> _get_rfft_plan(int n) {
    if (isprime(n)) {
        return std::make_shared<PrimesFftR>(n);
    }
    if (n % 2 == 0) {
        return std::make_shared<RealFftPlan>(n);
    }
    return std::make_shared<FactorFFTPlanR>(n);
}

}   // namespace

//-------------------------------------------------------------------------------------------------
std::shared_ptr<BaseFftPlanC> create_fft_plan(int n) {
    //dont cache small fft plans
    if ((n == 1) || (n == 2) || (n == 4) || (n == 8)) {
        return std::make_shared<SmallFftPow2C>(n);
    }

    //TODO: use weak_ptr cache to prevent duplication
    thread_local LRUCache<int, std::shared_ptr<BaseFftPlanC>> cache{FFT_CACHE_SIZE};
    if (!cache.exists(n)) {
        auto plan = _get_fft_plan(n);
        cache.put(n, plan);
        return plan;
    }
    return cache.get(n);
}

std::shared_ptr<BaseFftPlanR> create_rfft_plan(int n) {
    if ((n == 1) || (n == 2) || (n == 4) || (n == 8)) {
        return std::make_shared<SmallFftPow2R>(n);
    }

    thread_local LRUCache<int, std::shared_ptr<BaseFftPlanR>> cache{FFT_CACHE_SIZE};
    if (!cache.exists(n)) {
        auto plan = _get_rfft_plan(n);
        cache.put(n, plan);
        return plan;
    }
    return cache.get(n);
}

//-------------------------------------------------------------------------------------------------
FftPlan::FftPlan(int n)
  : _d{create_fft_plan(n)} {
}

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

//-------------------------------------------------------------------------------------------------
FftPlanR::FftPlanR(int n)
  : _d{create_rfft_plan(n)} {
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

arr_cmplx rfft(const arr_real& x) {
    return fft(x);
}

arr_cmplx rfft(const arr_real& x, int n) {
    return fft(x, n);
}

}   // namespace dsplib
