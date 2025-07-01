#include <dsplib/fft.h>
#include <dsplib/ifft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

#include "fft/cmplx-ifft.h"
#include "fft/fact-fft.h"
#include "fft/primes-fft.h"
#include "fft/factory.h"
#include "fft/pow2-fft.h"
#include "fft/real-fft.h"
#include "fft/real-ifft.h"
#include "fft/small-fft.h"
#include "internal/lru-cache.h"

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

std::shared_ptr<BaseIfftPlanC> _get_ifft_plan(int n) {
    return std::make_shared<CmplxIfftPlan>(n);
}

std::shared_ptr<BaseIfftPlanR> _get_irfft_plan(int n) {
    return std::make_shared<RealIfftPlan>(n);
}

}   // namespace

//-------------------------------------------------------------------------------------------------
std::shared_ptr<BaseFftPlanC> create_fft_plan(int n) {
    //dont cache small fft plans
    if (SmallFftC::is_supported(n)) {
        return std::make_shared<SmallFftC>(n);
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
    if (SmallFftR::is_supported(n)) {
        return std::make_shared<SmallFftR>(n);
    }

    thread_local LRUCache<int, std::shared_ptr<BaseFftPlanR>> cache{FFT_CACHE_SIZE};
    if (!cache.exists(n)) {
        auto plan = _get_rfft_plan(n);
        cache.put(n, plan);
        return plan;
    }
    return cache.get(n);
}

std::shared_ptr<BaseIfftPlanC> create_ifft_plan(int n) {
    thread_local LRUCache<int, std::shared_ptr<BaseIfftPlanC>> cache{FFT_CACHE_SIZE};
    if (!cache.exists(n)) {
        auto plan = _get_ifft_plan(n);
        cache.put(n, plan);
        return plan;
    }
    return cache.get(n);
}

std::shared_ptr<BaseIfftPlanR> create_irfft_plan(int n) {
    thread_local LRUCache<int, std::shared_ptr<BaseIfftPlanR>> cache{FFT_CACHE_SIZE};
    if (!cache.exists(n)) {
        auto plan = _get_irfft_plan(n);
        cache.put(n, plan);
        return plan;
    }
    return cache.get(n);
}

}   // namespace dsplib
