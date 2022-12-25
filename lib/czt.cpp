#include <dsplib/czt.h>
#include <dsplib/fft.h>
#include <dsplib/ifft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

namespace dsplib {

class CztPlanImpl
{
public:
    CztPlanImpl(int n, int m, cmplx_t w, cmplx_t a)
      : _n{n}
      , _m{m} {
        auto t = pow2(range(1 - n, max(m, n))) / 2;
        auto chirp = w ^ t;
        const int n2 = pow(2, nextpow2(m + n - 1));
        arr_cmplx cp = chirp.slice(n - 1, n + n - 1);
        auto pw = a ^ (-range(n));
        _cp = pw * cp;
        arr_cmplx dp = chirp.slice(0, m + n - 1);
        _ich = fft((1.0 / dp) | zeros(n2 - m - n + 1));
        _rp = chirp.slice(_n - 1, _m + _n - 1);
    }

    arr_cmplx solve(const arr_cmplx& x) const {
        auto xp = (x * _cp) | zeros(_ich.size() - x.size());
        auto r = ifft(fft(xp) * _ich);
        arr_cmplx tr = r.slice(_n - 1, _m + _n - 1);
        return tr * _rp;
    }

    int _n;
    int _m;
    arr_cmplx _ich;
    arr_cmplx _cp;
    arr_cmplx _rp;
};

CztPlan::CztPlan(int n, int m, cmplx_t w, cmplx_t a)
  : _d{std::make_shared<CztPlanImpl>(n, m, w, a)} {
}

arr_cmplx CztPlan::operator()(const arr_cmplx& x) const {
    return _d->solve(x);
}

arr_cmplx CztPlan::solve(const arr_cmplx& x) const {
    return _d->solve(x);
}

arr_cmplx czt(arr_cmplx x, int m, cmplx_t w, cmplx_t a) {
    CztPlan plan(x.size(), m, w, a);
    return plan(x);
}

}   // namespace dsplib
