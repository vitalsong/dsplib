#include "fft/fact-fft.h"
#include "fft/factory.h"

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
        assert(n >= 2);

        const auto fac = factor(n);
        if (fac.size() == 1) {
            _prime = true;
            //it is important to use the cache because there can be several identical FFTs
            _solver = create_fft_plan(n);
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

    [[nodiscard]] std::shared_ptr<BaseFftPlanC> solver() const noexcept {
        assert(_solver != nullptr);
        return _solver;
    }

private:
    int _n;
    bool _prime{false};
    PlanTree* _p{nullptr};
    PlanTree* _q{nullptr};
    std::shared_ptr<BaseFftPlanC> _solver;
};

namespace {

void _transpose(cmplx_t* x, cmplx_t* t, int n, int m) noexcept {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            t[j * n + i] = x[i * m + j];
        }
    }
    std::memcpy(x, t, n * m * sizeof(cmplx_t));
}

void _ctfft(const PlanTree* plan, cmplx_t* x, cmplx_t* mm, const cmplx_t* tw, int ntw) {
    const int n = plan->size();

    if (plan->is_prime()) {
        //TODO: separate in/out pointer
        plan->solver()->solve(x, x, n);
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