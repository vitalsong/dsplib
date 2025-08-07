#include <dsplib/fft.h>
#include <dsplib/ifft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

#include "fft/factory.h"
#include "fft/small-fft.h"
#include "internal/lru-cache.h"

#include <cassert>
#include <memory>

namespace dsplib {

namespace {

constexpr int FFT_CACHE_SIZE = DSPLIB_FFT_CACHE_SIZE;
static_assert(FFT_CACHE_SIZE >= 0);

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
            auto plan = internal::get_fft_plan(n);
            cache.put(n, plan);
            return plan;
        }
        return cache.get(n);
    } else {
        return internal::get_fft_plan(n);
    }
}

std::shared_ptr<FftPlanR> fft_plan_r(int n) {
    if (SmallFftR::is_supported(n)) {
        return std::make_shared<SmallFftR>(n);
    }

    if constexpr (FFT_CACHE_SIZE > 0) {
        thread_local LRUCache<int, std::shared_ptr<FftPlanR>> cache{FFT_CACHE_SIZE};
        if (!cache.exists(n)) {
            auto plan = internal::get_rfft_plan(n);
            cache.put(n, plan);
            return plan;
        }
        return cache.get(n);
    } else {
        return internal::get_rfft_plan(n);
    }
}

std::shared_ptr<IfftPlanC> ifft_plan_c(int n) {
    if constexpr (FFT_CACHE_SIZE > 0) {
        thread_local LRUCache<int, std::shared_ptr<IfftPlanC>> cache{FFT_CACHE_SIZE};
        if (!cache.exists(n)) {
            auto plan = internal::get_ifft_plan(n);
            cache.put(n, plan);
            return plan;
        }
        return cache.get(n);
    } else {
        return internal::get_ifft_plan(n);
    }
}

std::shared_ptr<IfftPlanR> ifft_plan_r(int n) {
    if constexpr (FFT_CACHE_SIZE > 0) {
        thread_local LRUCache<int, std::shared_ptr<IfftPlanR>> cache{FFT_CACHE_SIZE};
        if (!cache.exists(n)) {
            auto plan = internal::get_irfft_plan(n);
            cache.put(n, plan);
            return plan;
        }
        return cache.get(n);
    } else {
        return internal::get_irfft_plan(n);
    }
}

}   // namespace dsplib
