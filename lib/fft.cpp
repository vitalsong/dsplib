#include <dsplib/fft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>
#include <dsplib/czt.h>
#include <dsplib/throw.h>

#include "dft-tables.h"
#include "datacache.h"

#include <cassert>
#include <vector>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
//caching czt(n, n, exp(-2pi/n), 1) for nfft != 2^m
//TODO: optional disable caching
using CztPlanPtr = std::shared_ptr<CztPlan>;
static datacache<size_t, CztPlanPtr> g_czt_cache;

//-------------------------------------------------------------------------------------------------
static inline void _btrf(cmplx_t& x1, cmplx_t& x2, cmplx_t w) {
    w *= x2;
    x2 = x1 - w;
    x1 = x1 + w;
}

//-------------------------------------------------------------------------------------------------
//bit reverse array permutation
static arr_cmplx _bitreverse(const arr_cmplx& x, const std::vector<int32_t>& bitrev) {
    const int n = x.size();
    arr_cmplx r(n);
    for (int i = 0; i < n; ++i) {
        const auto k = bitrev[i];
        r[i] = x[k];
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
static arr_cmplx _fft2(const arr_cmplx& x_f, const arr_cmplx& coeffs, const std::vector<int32_t>& bitrev, int n) {
    assert(n % 2 == 0);
    assert(x_f.size() == n);
    assert(coeffs.size() == n);
    assert(bitrev.size() == n);

    auto x = _bitreverse(x_f, bitrev);

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
            pw = coeffs.data();
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
class FFT2Plan
{
public:
    explicit FFT2Plan(int n)
      : _n{n} {
        const int n2 = 1L << nextpow2(n);
        if (n != n2) {
            DSPLIB_THROW("Vector len is not power of 2");
        }
        _coeff = tables::dft_table(_n);
        _bitrev = tables::bitrev_table(_n);
    }

    [[nodiscard]] arr_cmplx solve(const arr_cmplx& x) const {
        assert(x.size() == _n);
        return _fft2(x, _coeff, _bitrev, _n);
    }

    [[nodiscard]] int size() const noexcept {
        return _n;
    }

private:
    const int _n{0};
    arr_cmplx _coeff;
    std::vector<int32_t> _bitrev;
};

//-------------------------------------------------------------------------------------------------
class FftPlanImpl
{
public:
    explicit FftPlanImpl(int n)
      : n_{n} {
        const int nfft2 = 1L << nextpow2(n);
        if (n == nfft2) {
            //n == 2^K
            auto plan = FFT2Plan(n);
            solve = [plan](const arr_cmplx& x) {
                return plan.solve(x);
            };
        } else {
            //n != 2^K
            if (!g_czt_cache.cached(n)) {
                cmplx_t w = expj(-2 * pi / n);
                auto plan = std::make_shared<CztPlan>(n, n, w);
                g_czt_cache.update(n, plan);
            }
            auto plan = g_czt_cache.get(n);
            solve = [plan](const arr_cmplx& x) {
                return plan->solve(x);
            };
        }
    }
    std::function<arr_cmplx(const arr_cmplx&)> solve;
    int n_;
};

//-------------------------------------------------------------------------------------------------
FftPlan::FftPlan(int n)
  : _d{std::make_shared<FftPlanImpl>(n)} {
}

//-------------------------------------------------------------------------------------------------
arr_cmplx FftPlan::solve(const arr_cmplx& x) const {
    return _d->solve(x);
}

//-------------------------------------------------------------------------------------------------
int FftPlan::size() const noexcept {
    return _d->n_;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fft(const arr_cmplx& arr) {
    FftPlan plan(arr.size());
    return plan(arr);
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
arr_cmplx fft(const arr_real& x) {
    using namespace std::complex_literals;

    const int n = x.size();
    auto nfft = 1L << nextpow2(n);
    if (nfft != n) {
        return fft(arr_cmplx(x));
    }

    //TODO: bad optimization

    arr_cmplx z(n / 2);
    //TODO: slice(0, n, 2),  slice(1, n, 2)
    for (int i = 0; i < (n / 2); ++i) {
        z[i].re = x[2 * i];
        z[i].im = x[2 * i + 1];
    }

    const auto Z = fft(z);

    // auto Zc = circshift(flip(conj(Z)), 1);
    auto Zc = conj(Z);
    std::reverse(Zc.begin() + 1, Zc.end());

    const arr_cmplx w = tables::dft_table(n).slice(0, n / 2);
    const auto Xe = (Z + Zc) * 0.5;
    const auto Xo = (Zc - Z) * 0.5i * w;
    return (Xe + Xo) | (Xe - Xo);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx IFftPlan::solve(const arr_cmplx& x) const {
    const int n = x.size();
    return conj(FftPlan::solve(conj(x)) / n);
}

//-------------------------------------------------------------------------------------------------
int IFftPlan::size() const noexcept {
    return FftPlan::size();
}

//-------------------------------------------------------------------------------------------------
arr_cmplx ifft(const arr_cmplx& x) {
    auto plan = IFftPlan(x.size());
    return plan(x);
}

}   // namespace dsplib
