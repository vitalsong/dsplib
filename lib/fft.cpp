#include <dsplib/fft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

namespace dsplib {

arr_cmplx fft(span_t<cmplx_t> x) {
    auto plan = fft_plan_c(x.size());
    return plan->solve(x);
}

arr_cmplx fft(span_t<cmplx_t> x, int n) {
    if (n == x.size()) {
        return fft(x);
    }
    if (n > x.size()) {
        return fft(zeropad(x, n));
    }
    return fft(x.slice(0, n));
}

arr_cmplx fft(span_t<real_t> x) {
    auto plan = fft_plan_r(x.size());
    return plan->solve(x);
}

arr_cmplx fft(span_t<real_t> x, int n) {
    if (n == x.size()) {
        return fft(x);
    }
    if (n > x.size()) {
        return fft(zeropad(x, n));
    }
    return fft(x.slice(0, n));
}

arr_cmplx rfft(span_t<real_t> x) {
    return fft(x);
}

arr_cmplx rfft(span_t<real_t> x, int n) {
    return fft(x, n);
}

}   // namespace dsplib
