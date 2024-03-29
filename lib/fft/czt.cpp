#include <dsplib/czt.h>
#include <dsplib/fft.h>
#include <dsplib/ifft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

namespace dsplib {

class CztPlanImpl
{
public:
    explicit CztPlanImpl(int n, int m, cmplx_t w, cmplx_t a)
      : _n{n}
      , _m{m} {
        assert(abs(abs(w) - 1.0) < 2 * eps());
        auto t = abs2(arange(1 - n, max(m, n))) / 2;
        arr_cmplx chirp(t.size());
        const auto w_a = angle(w);
        for (int i = 0; i < t.size(); ++i) {
            chirp[i] = expj(w_a * t[i]);
        }

        const int n2 = std::pow(2, nextpow2(m + n - 1));
        _cp = chirp.slice(n - 1, n + n - 1);

        if (abs(a - 1) > eps(a.re)) {
            const auto pw = power(a, -arange(n));
            _cp *= pw;
        }

        const arr_cmplx dp = chirp.slice(0, m + n - 1);
        _ich = fft((1.0 / dp) | zeros(n2 - m - n + 1));
        _rp = chirp.slice(_n - 1, _m + _n - 1);
    }

    [[nodiscard]] arr_cmplx solve(const arr_cmplx& x) const {
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

arr_cmplx CztPlan::solve(const arr_cmplx& x) const {
    return _d->solve(x);
}

int CztPlan::size() const noexcept {
    return _d->_n;
}

arr_cmplx czt(arr_cmplx x, int m, cmplx_t w, cmplx_t a) {
    CztPlan plan(x.size(), m, w, a);
    return plan(x);
}

}   // namespace dsplib
