#pragma once

#include <dsplib.h>
#include <gtest/gtest.h>

constexpr dsplib::real_t EQ_ABS_ERR = 1e-7;

using namespace std::complex_literals;

template<typename T1, typename T2>
void ASSERT_CMPLX_EQ(const T1& val1, const T2& val2) {
    ASSERT_EQ(val1.re, val2.re);
    ASSERT_EQ(val1.im, val2.im);
}

inline void ASSERT_CMPLX_NEAR(dsplib::cmplx_t x1, dsplib::cmplx_t x2, dsplib::real_t v = EQ_ABS_ERR) {
    ASSERT_NEAR(x1.re, x2.re, v);
    ASSERT_NEAR(x1.im, x2.im, v);
}

template<typename T1, typename T2>
void ASSERT_EQ_ARR_INT(const T1& r1, const T2& r2) {
    dsplib::arr_int x1(r1);
    dsplib::arr_int x2(r2);
    ASSERT_EQ(x1.size(), x2.size());
    if (x1.empty()) {
        return;
    }
    const auto err = dsplib::max(dsplib::abs(x1 - x2));
    ASSERT_EQ(err, 0);
}

template<typename T1, typename T2>
void ASSERT_EQ_ARR_REAL(const T1& r1, const T2& r2, dsplib::real_t max_err = EQ_ABS_ERR) {
    dsplib::arr_real x1(r1);
    dsplib::arr_real x2(r2);
    ASSERT_EQ(x1.size(), x2.size());
    if (x1.empty()) {
        return;
    }
    const auto err = dsplib::max(dsplib::abs(x1 - x2));
    ASSERT_LE(err, max_err);
}

template<typename T1, typename T2>
void ASSERT_EQ_ARR_CMPLX(const T1& r1, const T2& r2, dsplib::real_t max_err = EQ_ABS_ERR) {
    dsplib::arr_cmplx x1(r1);
    dsplib::arr_cmplx x2(r2);
    ASSERT_EQ(x1.size(), x2.size());
    if (x1.empty()) {
        return;
    }
    const auto err = dsplib::max(dsplib::abs(x1 - x2));
    ASSERT_LE(err, max_err);
}

namespace dsplib {

struct Harm
{
    real_t freq{0};
    real_t amp{0};
    real_t snr{0};
};

template<typename T>
Harm harm_analyze(const base_array<T>& x, int winlen = 1024) {
    winlen = 1L << nextpow2(winlen);
    auto [pxx, _] = welch(x, window::kaiser(winlen, 38), (winlen / 2), winlen, SpectrumType::Power);
    auto harm = thd(zeropad(pxx, winlen), 2, false, SinadType::Power);
    Harm res;
    res.amp = harm.harmpow[0];
    res.freq = harm.harmfreq[0] * 2;
    res.snr = snr(pxx, 1, false, SinadType::Power);
    return res;
}

}   // namespace dsplib