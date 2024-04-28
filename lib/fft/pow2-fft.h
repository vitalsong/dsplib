#pragma once

#include <dsplib/fft.h>
#include <dsplib/math.h>

#include "fft/dft-tables.h"

namespace dsplib {

class Pow2FftPlan : public BaseFftPlanC
{
public:
    explicit Pow2FftPlan(int n)
      : n_{n} {
        const auto tb = fft_tables(n);
        bitrev_ = tb.bitrev;
        coeffs_ = _cos_to_cmplx(tb.coeffs.data(), tb.size);
    }

    [[nodiscard]] arr_cmplx solve(const arr_cmplx& x) const final {
        const int n = x.size();
        arr_cmplx y(n);
        solve(x.data(), y.data(), n);
        return y;
    }

    void solve(const cmplx_t* x, cmplx_t* y, int n) const final {
        _fft(x, y, n);
    }

    [[nodiscard]] int size() const noexcept final {
        return n_;
    }

private:
    //cast cos table to complex for n==2^K
    static std::vector<cmplx_t> _cos_to_cmplx(const real_t* w, int n) {
        assert(ispow2(n));
        const int n4 = (n / 4);
        const int ms = (n - 1);
        std::vector<cmplx_t> r(n);
        for (int i = 0; i < n; ++i) {
            r[i].re = w[i];
            const int k = (i + n4) & ms;
            r[i].im = w[k];
        }
        return r;
    }

    //bit reverse array permutation
    static void _bitreverse(const cmplx_t* x, cmplx_t* y, const int32_t* bitrev, int n) noexcept {
        const int n2 = n / 2;
        for (int i = 0; i < n2; ++i) {
            const auto kl = bitrev[i];
            const auto kr = kl + 1;
            y[i] = x[kl];
            y[n2 + i] = x[kr];
        }
    }

    static inline void _btrf(cmplx_t& x1, cmplx_t& x2, cmplx_t w) noexcept {
        w *= x2;
        x2 = x1 - w;
        x1 = x1 + w;
    }

    void _fft(const cmplx_t* in, cmplx_t* out, int n) const {
        assert((n == n_) && (n_ % 2) == 0);
        assert(in != out);

        //reverse sampling
        _bitreverse(in, out, bitrev_.data(), n);

        int h = 1;       ///< number of butterflies in clusters (and step between elements)
        int m = n / 2;   ///< number of clusters (and step for the butterfly table)
        int r = 2;       ///< number of elements (butterflies * 2) in clusters

        cmplx_t* px = nullptr;         ///< pointer to go through the signal vector
        const cmplx_t* pw = nullptr;   ///< pointer to go through the coefficient vector
        const int L = nextpow2(n);     ///< number of DFT runs

        //cascades
        for (int i = 0; i < L; ++i) {
            px = out;
            //clusters
            for (int j = 0; j < m; ++j) {
                pw = coeffs_.data();
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

    const int n_;
    std::vector<int32_t> bitrev_;
    std::vector<cmplx_t> coeffs_;
};

}   // namespace dsplib