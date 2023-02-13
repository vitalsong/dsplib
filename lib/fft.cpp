#include <dsplib/fft.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>
#include <dsplib/czt.h>

#include "dft-tables.h"
#include "datacache.h"

#include <cassert>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
//caching czt(n, n, exp(-2pi/n), 1) for nfft != 2^m
//TODO: optional disable caching
using CztPlanPtr = std::shared_ptr<CztPlan>;
static datacache<size_t, CztPlanPtr> g_czt_cache;

//-------------------------------------------------------------------------------------------------
//bit reverse array permutation
static void _reversal(cmplx_t* restrict w, const int32_t* restrict bitrev, int n) {
    for (int i = 0; i < n; ++i) {
        const int k = bitrev[i];
        std::swap(w[i], w[k]);
    }
}

//-------------------------------------------------------------------------------------------------
static inline void _btrf(cmplx_t& x1, cmplx_t& x2, cmplx_t w) {
    w *= x2;
    x2 = x1 - w;
    x1 = x1 + w;
}

//-------------------------------------------------------------------------------------------------
static void _fft2(cmplx_t* restrict x, const cmplx_t* restrict w, const int32_t* restrict bitrev, int n) {
    assert(n % 2 == 0);

    //reverse sampling
    _reversal(x, bitrev, n);

    int h = 1;       ///< number of butterflies in clusters (and step between elements)
    int m = n / 2;   ///< number of clusters (and step for the butterfly table)
    int r = 2;       ///< number of elements (butterflies * 2) in clusters

    cmplx_t* px;                 ///< pointer to go through the signal vector
    const cmplx_t* pw;           ///< pointer to go through the coefficient vector
    const int L = nextpow2(n);   ///< number of DFT runs

    //cascades
    for (int i = 0; i < L; ++i) {
        px = x;
        //clusters
        for (int j = 0; j < m; ++j) {
            pw = w;
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
}

//-------------------------------------------------------------------------------------------------
class FftPlanImpl
{
public:
    explicit FftPlanImpl(int n)
      : n_{n} {
        const int n2 = 1L << nextpow2(n);
        if (n == n2) {
            //n == 2^K
            auto brev = tables::bitrev_table(n);
            auto coeff = tables::dft_table(n);
            solve = [brev, coeff, n](const arr_cmplx& x) {
                arr_cmplx r = x;
                _fft2(r.data(), coeff->data(), brev->data(), n);
                return r;
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
arr_cmplx FftPlan::operator()(const arr_cmplx& x) const {
    return _d->solve(x);
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

}   // namespace dsplib
