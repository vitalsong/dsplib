#include <dsplib/czt.h>
#include <dsplib/fft.h>
#include <dsplib/ifft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

#include <memory>

namespace dsplib {

class CztPlanImpl
{
public:
    explicit CztPlanImpl(int n, int m, cmplx_t w, cmplx_t a)
      : _n{n}
      , _m{m} {
        assert(abs(abs(w) - 1.0) < 2 * eps());
        const auto t = abs2(arange(1 - n, max(m, n))) / 2;
        arr_cmplx chirp(t.size());
        const auto w_a = angle(w);
        for (int i = 0; i < t.size(); ++i) {
            chirp[i] = expj(w_a * t[i]);
        }

        //TODO: find the nearest well-factorized size (not power of 2)
        const int n2 = (int)std::pow(2, nextpow2(m + n - 1));
        _cp = chirp.slice(n - 1, n + n - 1);

        assert(ispow2(n2));

        //use only _fft2 with conj
        _fft2 = fft_plan_c(n2);
        _ifft2 = ifft_plan_c(n2);

        if (abs(a - 1) > eps(a.re)) {
            const auto pw = power(a, -arange(n));
            _cp *= pw;
        }

        const arr_cmplx dp = chirp.slice(0, m + n - 1);
        _ich = _fft2->solve(zeropad((1.0 / dp), n2));
        _rp = chirp.slice(_n - 1, _m + _n - 1);
    }

    void solve(span_t<cmplx_t> x, mut_span_t<cmplx_t> r) const {
        DSPLIB_ASSERT(x.size() == _n, "input size must be equal CZT base");
        arr_cmplx xp(_fft2->size());
        for (int i = 0; i < _n; ++i) {
            xp[i] = x[i] * _cp[i];
        }

        _fft2->solve(inplace(xp));
        xp *= _ich;
        _ifft2->solve(inplace(xp));

        for (int i = 0; i < _n; ++i) {
            r[i] = xp[_n - 1 + i] * _rp[i];
        }
    }

    const int _n;
    const int _m;
    arr_cmplx _ich;
    arr_cmplx _cp;
    arr_cmplx _rp;
    std::shared_ptr<FftPlanC> _fft2;
    std::shared_ptr<IfftPlanC> _ifft2;
};

CztPlan::CztPlan(int n, int m, cmplx_t w, cmplx_t a)
  : _d{std::make_shared<CztPlanImpl>(n, m, w, a)} {
}

arr_cmplx CztPlan::solve(span_t<cmplx_t> x) const {
    arr_cmplx r(_d->_n);
    _d->solve(x, r);
    return r;
}

void CztPlan::solve(span_t<cmplx_t> x, mut_span_t<cmplx_t> r) const {
    _d->solve(x, r);
}

int CztPlan::size() const noexcept {
    return _d->_n;
}

arr_cmplx czt(span_t<cmplx_t> x, int m, cmplx_t w, cmplx_t a) {
    CztPlan plan(x.size(), m, w, a);
    return plan.solve(x);
}

}   // namespace dsplib
