#include <dsplib/fft.h>
#include <dsplib/ifft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

#include "fft/factory.h"
#include "fft/cmplx-ifft.h"
#include "fft/fact-fft.h"
#include "fft/primes-fft.h"
#include "fft/pow2-fft.h"
#include "fft/real-fft.h"
#include "fft/real-ifft.h"

#include <cassert>
#include <memory>

namespace dsplib::internal {

//TODO: common plan for complex fft/ifft

std::shared_ptr<FftPlanC> get_fft_plan(int n) {
    if (ispow2(n)) {
        return std::make_shared<Pow2FftPlan>(n);
    }
    if (isprime(n)) {
        return std::make_shared<PrimesFftC>(n);
    }
    return std::make_shared<FactorFFTPlan>(n);
}

std::shared_ptr<FftPlanR> get_rfft_plan(int n) {
    if (n % 2 == 0) {
        return std::make_shared<RealFftPlan>(n);
    }
    if (isprime(n)) {
        return std::make_shared<PrimesFftR>(n);
    }
    return std::make_shared<FactorFFTPlanR>(n);
}

std::shared_ptr<IfftPlanC> get_ifft_plan(int n) {
    return std::make_shared<CmplxIfftPlan>(n);
}

std::shared_ptr<IfftPlanR> get_irfft_plan(int n) {
    return std::make_shared<RealIfftPlan>(n);
}

}   // namespace dsplib::internal
