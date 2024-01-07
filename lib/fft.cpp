#include <dsplib/fft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>
#include <dsplib/czt.h>

#include "dft-tables.h"
#include "lru-cache.h"

#include <cassert>

namespace dsplib {

namespace {

constexpr int FFT_CACHE_SIZE = DSPLIB_FFT_CACHE_SIZE;

//-------------------------------------------------------------------------------------------------
//cast cos table to complex for n==2^K
arr_cmplx _cos_to_cmplx(const real_t* w, int n) {
    assert(ispow2(n));
    const int n4 = (n / 4);
    const int ms = (n - 1);
    arr_cmplx r(n);
    for (int i = 0; i < n; ++i) {
        r[i].re = w[i];
        const int k = (i + n4) & ms;
        r[i].im = w[k];
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
//bit reverse array permutation
arr_cmplx _bitreverse(const arr_cmplx& x, const std::vector<int32_t>& bitrev) noexcept {
    const int n = x.size();
    const int n2 = n / 2;
    assert(bitrev.size() == n2);
    arr_cmplx r(n);
    for (int i = 0; i < n2; ++i) {
        const auto kl = bitrev[i];
        const auto kr = kl + 1;
        r[i] = x[kl];
        r[n2 + i] = x[kr];
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
inline void _btrf(cmplx_t& x1, cmplx_t& x2, cmplx_t w) {
    w *= x2;
    x2 = x1 - w;
    x1 = x1 + w;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx _fft_radix2(const arr_cmplx& in, const arr_cmplx& w, const std::vector<int32_t>& bitrev, int n) {
    assert(n % 2 == 0);
    assert(in.size() == n);
    assert(w.size() == n);

    //reverse sampling
    auto x = _bitreverse(in, bitrev);

    int h = 1;       ///< number of butterflies in clusters (and step between elements)
    int m = n / 2;   ///< number of clusters (and step for the butterfly table)
    int r = 2;       ///< number of elements (butterflies * 2) in clusters

    cmplx_t* px;                 ///< pointer to go through the signal vector
    const cmplx_t* pw;           ///< pointer to go through the coefficient vector
    const int L = nextpow2(n);   ///< number of DFT runs

    //cascades
    for (int i = 0; i < L; ++i) {
        px = x.data();
        //clusters
        for (int j = 0; j < m; ++j) {
            pw = w.data();
            //butterflies
            for (int k = 0; k < h; ++k) {
                _btrf(px[k], px[k + h], *pw);
                pw += m;
            }
            px += r;
        }

        //next cascade
        m /= 2;
        h *= 2;
        r *= 2;
    }

    return x;
}

//-------------------------------------------------------------------------------------------------
class Fft2Plan : public BaseFftPlanC
{
public:
    explicit Fft2Plan(int n)
      : n_{n} {
        const auto tb = fft_tables(n);
        bitrev_ = tb.bitrev;
        coeffs_ = _cos_to_cmplx(tb.coeffs.data(), tb.size);
    }

    [[nodiscard]] arr_cmplx solve(const arr_cmplx& x) const final {
        return _fft_radix2(x, coeffs_, bitrev_, n_);
    }

    [[nodiscard]] int size() const noexcept final {
        return n_;
    }

private:
    const int n_;
    std::vector<int32_t> bitrev_;
    arr_cmplx coeffs_;
};

//-------------------------------------------------------------------------------------------------
std::shared_ptr<BaseFftPlanC> _get_fft_plan(int n) {
    //n!=2^K
    if (!ispow2(n)) {
        if (isprime(n)) {
            const cmplx_t w = expj(-2 * pi / n);
            return std::make_shared<CztPlan>(n, n, w);
        }
        return std::make_shared<FactorFFTPlan>(n);
    }
    return std::make_shared<Fft2Plan>(n);
}

//-------------------------------------------------------------------------------------------------
class CztPlanR : public BaseFftPlanR
{
public:
    explicit CztPlanR(int n)
      : _plan(n) {
    }

    [[nodiscard]] arr_cmplx solve(const arr_real& x) const final {
        return _plan.solve(arr_cmplx(x));
    }

    [[nodiscard]] int size() const noexcept final {
        return _plan.size();
    }

private:
    FftPlan _plan;
};

//-------------------------------------------------------------------------------------------------
class Fft2PlanR : public BaseFftPlanR
{
public:
    explicit Fft2PlanR(int n)
      : n_{n}
      , fft_{n / 2} {
        assert(ispow2(n));
        w_ = _cos_to_cmplx(fft_tables(n).coeffs.data(), n).slice(0, n / 2);
    }

    [[nodiscard]] arr_cmplx solve(const arr_real& x) const final {
        using namespace std::complex_literals;
        assert(x.size() == n_);
        const int n2 = n_ / 2;

        arr_cmplx z(reinterpret_cast<const cmplx_t*>(x.data()), n2);
        const auto Z = fft_.solve(z * 0.5);

        arr_cmplx res(n_);

        {
            const auto Xe = Z[0] + conj(Z[0]);
            const auto Xo = (conj(Z[0]) - Z[0]) * w_[0];
            res[0].re = Xe.re - Xo.im;
            res[0].im = Xe.im + Xo.re;
        }

        for (int i = 1; i < n2; ++i) {
            const auto Zc = conj(Z[n2 - i]);
            const auto Xe = Z[i] + Zc;
            const auto Xo = (Zc - Z[i]) * w_[i];
            res[i].re = Xe.re - Xo.im;
            res[i].im = Xe.im + Xo.re;
            res[n_ - i].re = res[i].re;
            res[n_ - i].im = -res[i].im;
        }

        {
            const auto Xe = Z[0] + conj(Z[0]);
            const auto Xo = conj(Z[0]) - Z[0];
            res[n2] = Xe.re + Xo.im;
        }

        return res;
    }

    [[nodiscard]] int size() const noexcept final {
        return n_;
    }

private:
    const int n_;
    Fft2Plan fft_;
    arr_cmplx w_;
};

//-------------------------------------------------------------------------------------------------
std::shared_ptr<BaseFftPlanR> _get_rfft_plan(int n) {
    //n!=2^K
    //TODO: add FactFFTPlanR
    if (!ispow2(n)) {
        return std::make_shared<CztPlanR>(n);
    }
    return std::make_shared<Fft2PlanR>(n);
}

}   // namespace

//-------------------------------------------------------------------------------------------------
FftPlan::FftPlan(int n) {
    thread_local LRUCache<int, std::shared_ptr<BaseFftPlanC>> cache{FFT_CACHE_SIZE};
    if (!cache.exists(n)) {
        cache.put(n, _get_fft_plan(n));
    }
    _d = cache.get(n);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fft(const arr_cmplx& x) {
    auto plan = FftPlan(x.size());
    return plan(x);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fft(const arr_cmplx& x, int n) {
    if (n == x.size()) {
        return fft(x);
    }

    if (n > x.size()) {
        return fft(zeropad(x, n));
    }

    return fft(x.slice(0, n));
}

//-------------------------------------------------------------------------------------------------
FftPlanR::FftPlanR(int n) {
    thread_local LRUCache<int, std::shared_ptr<BaseFftPlanR>> cache{FFT_CACHE_SIZE};
    if (!cache.exists(n)) {
        cache.put(n, _get_rfft_plan(n));
    }
    _d = cache.get(n);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fft(const arr_real& x) {
    auto plan = FftPlanR(x.size());
    return plan(x);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fft(const arr_real& x, int n) {
    if (n == x.size()) {
        return fft(x);
    }

    if (n > x.size()) {
        return fft(zeropad(x, n));
    }

    return fft(x.slice(0, n));
}

}   // namespace dsplib
