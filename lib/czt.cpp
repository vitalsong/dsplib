#include <dsplib/fft.h>
#include <dsplib/ifft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

namespace dsplib {

arr_cmplx czt(arr_cmplx x, real_t m, cmplx_t w, cmplx_t a) {
    const int n = x.size();

    // chirp = w ** (arange(1 - n, max(m, n)) ** 2 / 2.0)
    auto t = (range(1 - n, max(m, n)) ^ 2) / 2;
    auto chirp = w ^ t;

    //xp = append(x * a ** -arange(n) * chirp[n - 1 : n + n - 1], zeros(N2 - n))
    const int n2 = pow(2, nextpow2(m + n - 1));
    arr_cmplx cp = chirp.slice(n - 1, n + n - 1);
    auto pw = a ^ (-range(n));
    auto xp = (x * pw * cp) | zeros(n2 - n);

    // ichirpp = append(1 / chirp[: m + n - 1], zeros(N2 - (m + n - 1)))
    cp = chirp.slice(0, m + n - 1);
    auto ichirpp = (1.0 / cp) | zeros(n2 - m - n + 1);

    auto r = ifft(fft(xp) * fft(ichirpp));
    arr_cmplx tr = r.slice(n - 1, m + n - 1);
    cp = chirp.slice(n - 1, m + n - 1);
    return tr * cp;
}

}   // namespace dsplib
