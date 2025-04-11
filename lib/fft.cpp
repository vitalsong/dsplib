#include <dsplib/fft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

namespace dsplib {

arr_cmplx fft(const arr_cmplx& x) {
    auto plan = fft_plan_c(x.size());
    return plan->solve(x);
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
    auto plan = fft_plan_r(x.size());
    return plan->solve(x);
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
