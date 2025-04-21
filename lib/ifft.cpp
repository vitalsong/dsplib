#include <dsplib/ifft.h>
#include <dsplib/fft.h>
#include <dsplib/math.h>

namespace dsplib {

//---------------------------------------------------------------------------------
arr_cmplx ifft(const arr_cmplx& x) {
    auto plan = ifft_plan_c(x.size());
    return plan->solve(x);
}

arr_real irfft(const arr_cmplx& x) {
    return irfft(x, x.size());
}

arr_real irfft(const arr_cmplx& x, int n) {
    auto plan = ifft_plan_r(n);
    return plan->solve(x);
}

}   // namespace dsplib
