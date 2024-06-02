#include "fft/fact-fft.h"

#include <dsplib/math.h>
#include <dsplib/utils.h>

//For an explanation of the algorithm, see the article:
//https://numericalrecipes.wordpress.com/2009/05/29/the-cooley-tukey-fft-algorithm-for-general-factorizations/

namespace dsplib {

//constructing a factorization plan
//example for n=120, factor is (2, 2, 2, 3, 5) and plan is 120 -> (8) | (15) -> (8) | (3 | 5)
class PlanTree
{
public:
    explicit PlanTree(int n)
      : _n{n} {
        DSPLIB_ASSERT(n >= 2, "FFT plan size error");

        //use Pow2FFT solver
        if (ispow2(n)) {
            _solver = fft_plan_c(n);
            return;
        }

        const auto fac = _factor(n);

        //use PrimeFFT solver
        if (fac.size() == 1) {
            //it is important to use the cache because there can be several identical FFTs
            _solver = fft_plan_c(n);
            return;
        }

        auto P = fac[0];
        const auto qn = std::sqrt(n);
        for (size_t i = 1; i < fac.size(); ++i) {
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
        assert(has_next());
        return _q;
    }

    [[nodiscard]] PlanTree* p_plan() const noexcept {
        assert(has_next());
        return _p;
    }

    bool has_next() const noexcept {
        return (_q != nullptr) && (_p != nullptr);
    }

    [[nodiscard]] std::shared_ptr<FftPlanC> solver() const noexcept {
        assert(_solver != nullptr);
        return _solver;
    }

private:
    //factorization with extract 2^n component, example (2, 2, 2, 3) -> (8, 3)
    static std::vector<int> _factor(int n) noexcept {
        const int pn = n;
        while (n % 2 == 0) {
            n /= 2;
        }

        std::vector<int> fac;
        if (n != pn) {
            fac.push_back(pn / n);
        }

        if (n == 1) {
            return fac;
        }

        const auto fc = factor(n);
        fac.insert(fac.end(), fc.begin(), fc.end());
        std::sort(fac.begin(), fac.end());
        return fac;
    }

    const int _n;
    PlanTree* _p{nullptr};
    PlanTree* _q{nullptr};
    std::shared_ptr<FftPlanC> _solver;
};

namespace {

void _transpose(cmplx_t* restrict x, cmplx_t* restrict mem, int n, int m) noexcept {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            mem[j * n + i] = x[i * m + j];
        }
    }
    std::memcpy(x, mem, n * m * sizeof(cmplx_t));
}

/**
 * @brief PlanTree processing
 * 
 * @param plan fft plan node
 * @param x source signal
 * @param mem calculations buffer
 * @param tw complex exponent exp(-2 * pi * (0:n-1) / n)
 * @param head_n basic FFT size (head node)
 */
void _facfft(const PlanTree* plan, cmplx_t* restrict x, cmplx_t* restrict mem, const cmplx_t* restrict tw, int head_n) {
    const int n = plan->size();

    if (!plan->has_next()) {
        //TODO: span
        plan->solver()->solve(x, mem, n);
        std::memcpy(x, mem, n * sizeof(cmplx_t));
        return;
    }

    const auto* qplan = plan->q_plan();
    const auto* pplan = plan->p_plan();

    const int qlen = qplan->size();
    const int plen = pplan->size();

    _transpose(x, mem, plen, qlen);

    //inner fft (size P)
    for (int k = 0; k < qlen; ++k) {
        auto* px = x + (k * plen);
        _facfft(pplan, px, mem, tw, head_n);
    }

    //multiple by twiddle (ignore first row and column, because it is always 1.0)
    const int decim = head_n / (plen * qlen);
    for (int p = 1; p < plen; ++p) {
        for (int q = 1; q < qlen; ++q) {
            const int idx = q * p * decim;
            x[q * plen + p] *= tw[idx];
        }
    }

    //outer fft (size Q)
    _transpose(x, mem, qlen, plen);
    for (int k = 0; k < plen; ++k) {
        auto* px = x + (k * qlen);
        _facfft(qplan, px, mem, tw, head_n);
    }

    _transpose(x, mem, plen, qlen);
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

[[nodiscard]] arr_cmplx FactorFFTPlan::solve(span_t<cmplx_t> x) const {
    DSPLIB_ASSERT(x.size() == _n, "input vector size is not equal fft size");
    arr_cmplx r(x);   //TODO: remove copy
    _facfft(_plan.get(), r.data(), _px.data(), _twiddle.data(), _n);
    return r;
}

[[nodiscard]] int FactorFFTPlan::size() const noexcept {
    return _n;
}

}   // namespace dsplib