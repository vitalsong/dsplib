#include "fact-fft.h"

#include <dsplib/math.h>
#include <dsplib/utils.h>

//For an explanation of the algorithm, see the article:
//https://numericalrecipes.wordpress.com/2009/05/29/the-cooley-tukey-fft-algorithm-for-general-factorizations/

namespace dsplib {

//constructing a factorization plan
//example for n=120, factor is (2, 2, 2, 3, 5) and plan is 120 -> (8) | (15) -> (2 | (2 | 2))) | (3 | 5)
//todo: allocate a separate 2^K plan
class PlanTree
{
public:
    explicit PlanTree(int n)
      : _n{n} {
        if (n < 2) {
            throw std::runtime_error("Plan size must be greater than 2");
        }

        const auto fac = factor(n);
        if (fac.size() == 1) {
            _prime = true;
            return;
        }

        auto P = fac[0];
        const auto qn = std::sqrt(n);
        for (int i = 1; i < fac.size(); ++i) {
            if (P * fac[i] > qn) {
                break;
            }
            P *= fac[i];
        }

        auto Q = n / P;
        _p = new PlanTree(P);
        _q = new PlanTree(Q);
    }

    ~PlanTree() {
        if (_p) {
            delete _p;
        }
        if (_q) {
            delete _q;
        }
    }

    int size() const noexcept {
        return _n;
    }

    [[nodiscard]] PlanTree* q_plan() const noexcept {
        return _q;
    }

    [[nodiscard]] PlanTree* p_plan() const noexcept {
        return _p;
    }

    bool is_prime() const noexcept {
        return _prime;
    }

private:
    int _n;
    bool _prime{false};
    PlanTree* _p{nullptr};
    PlanTree* _q{nullptr};
};

namespace {

constexpr int MAX_DFT_SIZE = 31;

void _dft_n2(cmplx_t* x) noexcept {
    cmplx_t y0, y1;

    y0.re = x[0].re + x[1].re;
    y0.im = x[0].im + x[1].im;

    y1.re = (x[0].re - x[1].re);
    y1.im = (x[0].im - x[1].im);

    x[0] = y0;
    x[1] = y1;
}

void _dft_n3(cmplx_t* x) noexcept {
    cmplx_t y0, y1, y2;
    constexpr real_t c = -0.5;
    constexpr real_t d = 0.866025403784439;

    y0.re = x[0].re + x[1].re + x[2].re;
    y0.im = x[0].im + x[1].im + x[2].im;

    const real_t re1_c = x[1].re * c;
    const real_t im1_d = x[1].im * d;
    const real_t re2_c = x[2].re * c;
    const real_t im2_d = x[2].im * d;
    y1.re = x[0].re + (re1_c + im1_d) + (re2_c - im2_d);
    y2.re = x[0].re + (re1_c - im1_d) + (re2_c + im2_d);

    const real_t re1_d = x[1].re * d;
    const real_t im1_c = x[1].im * c;
    const real_t re2_d = x[2].re * d;
    const real_t im2_c = x[2].im * c;
    y1.im = x[0].im + (-re1_d + im1_c) + (re2_d + im2_c);
    y2.im = x[0].im + (re1_d + im1_c) + (-re2_d + im2_c);

    x[0] = y0;
    x[1] = y1;
    x[2] = y2;
}

void _dft_slow(cmplx_t* x, int n, const cmplx_t* tw) noexcept {
    cmplx_t y[MAX_DFT_SIZE];
    std::memset(&y[0].re, 0, sizeof(y));

    for (int i = 0; i < n; ++i) {
        y[0] += x[i];
    }

    for (int k = 1; k < n; ++k) {
        int iw = 0;
        for (int i = 0; i < n; ++i) {
            y[k] += x[i] * tw[iw];
            iw += k;
            iw = (iw < n) ? iw : (iw - n);
        }
    }

    std::memcpy(x, y, n * sizeof(cmplx_t));
}

void _transpose(cmplx_t* x, cmplx_t* t, int n, int m) noexcept {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            t[j * n + i] = x[i * m + j];
        }
    }
    std::memcpy(x, t, n * m * sizeof(cmplx_t));
}

void _dft(cmplx_t* x, int n) {
    using namespace std::complex_literals;

    if (n == 2) {
        _dft_n2(x);
        return;
    }

    if (n == 3) {
        _dft_n3(x);
        return;
    }

    if (n == 5) {
        const cmplx_t tw[5] = {1.00000000000000 + 0.00000000000000i, 0.309016994374947 - 0.951056516295154i,
                               -0.809016994374947 - 0.587785252292473i, -0.809016994374948 + 0.587785252292473i,
                               0.309016994374947 + 0.951056516295154i};
        _dft_slow(x, n, tw);
        return;
    }

    if (n == 7) {
        const cmplx_t tw[7] = {1.00000000000000 + 0.00000000000000i,    0.623489801858734 - 0.781831482468030i,
                               -0.222520933956314 - 0.974927912181824i, -0.900968867902419 - 0.433883739117558i,
                               -0.900968867902419 + 0.433883739117558i, -0.222520933956315 + 0.974927912181824i,
                               0.623489801858733 + 0.781831482468030i};
        _dft_slow(x, n, tw);
        return;
    }

    if (n == 11) {
        const cmplx_t tw[11] = {1.00000000000000 + 0.00000000000000i,    0.841253532831181 - 0.540640817455598i,
                                0.415415013001886 - 0.909631995354518i,  -0.142314838273285 - 0.989821441880933i,
                                -0.654860733945285 - 0.755749574354258i, -0.959492973614497 - 0.281732556841430i,
                                -0.959492973614498 + 0.281732556841429i, -0.654860733945285 + 0.755749574354258i,
                                -0.142314838273285 + 0.989821441880933i, 0.415415013001886 + 0.909631995354519i,
                                0.841253532831181 + 0.540640817455597i};
        _dft_slow(x, n, tw);
        return;
    }

    if (n <= MAX_DFT_SIZE) {
        //TODO: use base twiddle factors table or use cache
        const arr_cmplx tw = expj(-2 * pi * arange(n) / n);
        _dft_slow(x, n, tw.data());
        return;
    }

    //TODO: noexcept?
    //TODO: call czt without copy, instance czt plan for last factor
    const auto y = fft(arr_cmplx(x, n));
    std::memcpy(x, y.data(), n * sizeof(cmplx_t));
}

void _ctfft(const PlanTree* plan, cmplx_t* x, cmplx_t* mm, const cmplx_t* tw, int ntw) {
    const int n = plan->size();

    if (plan->is_prime()) {
        _dft(x, n);
        return;
    }

    const auto* qplan = plan->q_plan();
    const auto* pplan = plan->p_plan();

    const int Q = qplan->size();
    const int P = pplan->size();

    //TODO: ignore this transpose and previous?
    _transpose(x, mm, P, Q);

    //inner fft (size P)
    for (int k = 0; k < Q; ++k) {
        auto* px = x + (k * P);
        _ctfft(pplan, px, mm, tw, ntw);
    }

    //multiple by twiddle (ignore first row and column, because it is always 1.0)
    const int decim = ntw / (P * Q);
    for (int p = 1; p < P; ++p) {
        for (int q = 1; q < Q; ++q) {
            const int idx = q * p * decim;
            x[q * P + p] *= tw[idx];
        }
    }

    //outer fft (size Q)
    _transpose(x, mm, Q, P);
    for (int k = 0; k < P; ++k) {
        auto* px = x + (k * Q);
        _ctfft(qplan, px, mm, tw, ntw);
    }

    _transpose(x, mm, P, Q);
}

}   // namespace

//-----------------------------------------------------------------------------------------------------------------------------
FactorFFTPlan::FactorFFTPlan(int n)
  : _n{n}
  , _px(n) {
    DSPLIB_ASSERT(!isprime(n), "fft size must not be a prime number");
    _twiddle = expj(-2 * pi * arange(n) / n);   //TODO: only part of the table is needed
    _plan = std::make_shared<PlanTree>(n);
}

[[nodiscard]] arr_cmplx FactorFFTPlan::solve(const arr_cmplx& x) const {
    DSPLIB_ASSERT(x.size() == _n, "input vector size is not equal fft size");
    arr_cmplx r(x);
    _ctfft(_plan.get(), r.data(), _px.data(), _twiddle.data(), _n);
    return r;
}

[[nodiscard]] int FactorFFTPlan::size() const noexcept {
    return _n;
}

}   // namespace dsplib