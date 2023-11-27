#pragma once

#include <dsplib.h>
#include <gtest/gtest.h>

#define EQ_ABS_ERR (1e-7)

using namespace std::complex_literals;

//-------------------------------------------------------------------------------------------------
template<typename T1, typename T2>
static void ASSERT_CMPLX_EQ(const T1& val1, const T2& val2) {
    ASSERT_EQ(val1.re, val2.re);
    ASSERT_EQ(val1.im, val2.im);
}

static void ASSERT_CMPLX_NEAR(dsplib::cmplx_t x1, dsplib::cmplx_t x2, dsplib::real_t v = EQ_ABS_ERR) {
    ASSERT_NEAR(x1.re, x2.re, v);
    ASSERT_NEAR(x1.im, x2.im, v);
}

//-------------------------------------------------------------------------------------------------
template<typename T1, typename T2>
static void ASSERT_EQ_ARR_REAL(const T1& r1, const T2& r2, double max_err = EQ_ABS_ERR) {
    const dsplib::arr_real x1(r1);
    const dsplib::arr_real x2(r2);
    ASSERT_EQ(x1.size(), x2.size());
    if (x1.empty()) {
        return;
    }
    const auto err = dsplib::abs(x1 - x2);
    const auto err_v = dsplib::max(err);
    ASSERT_LE(err_v, max_err);
}

//-------------------------------------------------------------------------------------------------
template<typename T1, typename T2>
static void ASSERT_EQ_ARR_CMPLX(const T1& r1, const T2& r2, double max_err = EQ_ABS_ERR) {
    const dsplib::arr_cmplx x1(r1);
    const dsplib::arr_cmplx x2(r2);
    ASSERT_EQ(x1.size(), x2.size());
    if (x1.empty()) {
        return;
    }
    ASSERT_EQ_ARR_REAL(dsplib::real(x1), dsplib::real(x2), max_err);
    ASSERT_EQ_ARR_REAL(dsplib::imag(x1), dsplib::imag(x2), max_err);
}

namespace dsplib {

static arr_real calc_psd(const arr_real& x) {
    const int n = 1L << nextpow2(x.size());
    auto w = window::kaiser(x.size(), 38);
    w /= rms(w);
    const real_t u = real_t(x.size()) * n / 2;
    auto X = fft(x * w, n);
    X.slice(n / 2 + 1, n) = 0;
    const arr_real spec = abs2(X) / u;
    return spec;
}

static arr_real calc_psd(const arr_cmplx& x) {
    const int n = 1L << nextpow2(x.size());
    auto w = window::kaiser(x.size(), 38);
    w /= rms(w);
    const real_t u = real_t(x.size()) * n;
    const auto X = fft(x * w, n);
    const arr_real spec = abs2(X) / u;
    return spec;
}

struct Harm
{
    real_t freq{0};
    real_t amp{0};
    real_t snr{0};
};

template<typename T>
static Harm harm_analyze(const base_array<T>& x) {
    auto spec = calc_psd(x);
    const auto [pks, locs, wds] = findpeaks(spec, 1);
    Harm res;
    res.amp = pow2db(pks[0]);
    res.freq = locs[0] / spec.size();
    const int lp = std::round(locs[0] - wds[0] / 2);
    const int rp = std::round(locs[0] + wds[0] / 2);
    auto fund_pow = sum(*spec.slice(lp, rp + 1));
    res.snr = pow2db(fund_pow / (sum(spec) - fund_pow + eps()));
    return res;
}

}   // namespace dsplib