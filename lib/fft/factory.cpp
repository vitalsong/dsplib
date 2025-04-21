#include <dsplib/fft.h>
#include <dsplib/ifft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

#include "fft/cmplx-ifft.h"
#include "fft/fact-fft.h"
#include "fft/primes-fft.h"
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

static_assert(FFT_CACHE_SIZE >= 0);

//TODO: common plan for complex fft/ifft

std::shared_ptr<FftPlanC> _get_fft_plan(int n) {
    if (isprime(n)) {
        return std::make_shared<PrimesFftC>(n);
    }
    if (ispow2(n)) {
        return std::make_shared<Pow2FftPlan>(n);
    }
    return std::make_shared<FactorFFTPlan>(n);
}

std::shared_ptr<FftPlanR> _get_rfft_plan(int n) {
    if (isprime(n)) {
        return std::make_shared<PrimesFftR>(n);
    }
    if (n % 2 == 0) {
        return std::make_shared<RealFftPlan>(n);
    }
    return std::make_shared<FactorFFTPlanR>(n);
}

std::shared_ptr<IfftPlanC> _get_ifft_plan(int n) {
    return std::make_shared<CmplxIfftPlan>(n);
}

std::shared_ptr<IfftPlanR> _get_irfft_plan(int n) {
    return std::make_shared<RealIfftPlan>(n);
}

}   // namespace

//-------------------------------------------------------------------------------------------------
std::shared_ptr<FftPlanC> fft_plan_c(int n) {
    //dont cache small fft plans
    if (SmallFftC::is_supported(n)) {
        return std::make_shared<SmallFftC>(n);
    }

    if constexpr (FFT_CACHE_SIZE > 0) {
        //TODO: use weak_ptr cache to prevent duplication
        thread_local LRUCache<int, std::shared_ptr<FftPlanC>> cache{FFT_CACHE_SIZE};
        if (!cache.exists(n)) {
            auto plan = _get_fft_plan(n);
            cache.put(n, plan);
            return plan;
        }
        return cache.get(n);
    } else {
        return _get_fft_plan(n);
    }
}

std::shared_ptr<FftPlanR> fft_plan_r(int n) {
    if (SmallFftR::is_supported(n)) {
        return std::make_shared<SmallFftR>(n);
    }

    if constexpr (FFT_CACHE_SIZE > 0) {
        thread_local LRUCache<int, std::shared_ptr<FftPlanR>> cache{FFT_CACHE_SIZE};
        if (!cache.exists(n)) {
            auto plan = _get_rfft_plan(n);
            cache.put(n, plan);
            return plan;
        }
        return cache.get(n);
    } else {
        return _get_rfft_plan(n);
    }
}

std::shared_ptr<IfftPlanC> ifft_plan_c(int n) {
    if constexpr (FFT_CACHE_SIZE > 0) {
        thread_local LRUCache<int, std::shared_ptr<IfftPlanC>> cache{FFT_CACHE_SIZE};
        if (!cache.exists(n)) {
            auto plan = _get_ifft_plan(n);
            cache.put(n, plan);
            return plan;
        }
        return cache.get(n);
    } else {
        return _get_ifft_plan(n);
    }
}

std::shared_ptr<IfftPlanR> ifft_plan_r(int n) {
    if constexpr (FFT_CACHE_SIZE > 0) {
        thread_local LRUCache<int, std::shared_ptr<IfftPlanR>> cache{FFT_CACHE_SIZE};
        if (!cache.exists(n)) {
            auto plan = _get_irfft_plan(n);
            cache.put(n, plan);
            return plan;
        }
        return cache.get(n);
    } else {
        return _get_irfft_plan(n);
    }
}

}   // namespace dsplib
