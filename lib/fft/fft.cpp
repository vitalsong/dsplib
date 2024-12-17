#include <dsplib/fft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

#include "fft/factory.h"

#include <cassert>

namespace dsplib {

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
