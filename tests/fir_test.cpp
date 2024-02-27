#include "tests_common.h"
#include "ma-filter.h"
#include <cmath>

using namespace dsplib;

namespace {
const arr_real IR = fir1(31, 0.1);
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, FirAndOne) {
    auto flt = FirFilter(IR);
    auto x = zeros(IR.size());
    x[0] = 1;
    auto y = flt(x);
    ASSERT_EQ_ARR_REAL(IR, y);
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, FftFirAndOne) {
    auto flt = FftFilter(IR);
    auto x = zeros(2 * IR.size());
    x[0] = 1;
    arr_real y = flt(x).slice(0, IR.size());
    ASSERT_EQ_ARR_REAL(IR, y);
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, FftEqFir) {
    for (int nh = 1; nh <= 128; ++nh) {
        const auto ir_coeff = fir1(nh, 0.1);
        auto flt1 = FftFilter(ir_coeff);
        auto flt2 = FirFilter(ir_coeff);
        auto in = randn(10000);
        auto x1 = flt1(in);
        auto x2 = flt2(in);
        int n = min(x1.size(), x2.size());
        arr_real y1 = x1.slice(0, n);
        arr_real y2 = x2.slice(0, n);
        ASSERT_NE(n, 0);
        ASSERT_EQ_ARR_REAL(y1, y2);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, NonSymIR) {
    auto h = randn(103);
    auto flt = FirFilter(h);
    auto x = zeros(h.size());
    x[0] = 1;
    auto y = flt(x);
    ASSERT_EQ_ARR_REAL(h, y);
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, CmplxFftFir) {
    auto h = randn(100) + randn(100) * 1i;
    auto flt1 = FftFilter(h);
    auto flt2 = FirFilter(h);
    auto in = randn(10000) + randn(10000) * 1i;
    auto x1 = flt1(in);
    auto x2 = flt2(in);
    int n = min(x1.size(), x2.size());
    arr_cmplx y1 = x1.slice(0, n);
    arr_cmplx y2 = x2.slice(0, n);
    ASSERT_NE(n, 0);
    ASSERT_EQ_ARR_CMPLX(y1, y2);
}

//-------------------------------------------------------------------------------------------------
static arr_cmplx _get_bandpass_fir(int len, double f0, double f1) {
    arr_cmplx H = zeros(len);
    auto t0 = int(f0 * len);
    auto t1 = int(f1 * len);
    H.slice(t0, t1) = 1;
    arr_cmplx h = ifft(H);
    h = arr_cmplx(h.slice(len / 2, len)) | arr_cmplx(h.slice(0, len / 2));
    h *= window::hamming(len);
    return h;
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, Lms) {
    int M = IR.size();
    int L = 1000;
    auto flt = FirFilter(IR);
    auto x = randn(L);
    auto n = 0.01 * randn(L);
    auto d = flt(x) + n;

    auto mu_max = 2 / ((M + 1) * mean(x * x));
    auto mu = 0.05 * mu_max;
    auto adapt = LmsFilterR(M, mu);
    auto [y, e] = adapt(x, d);
    auto w = adapt.coeffs();
    ASSERT_LE(nmse(w, IR), 1e-3);
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, Rls) {
    int M = IR.size();
    int L = 1000;
    auto flt = FirFilter(IR);
    auto x = randn(L);
    auto n = 0.01 * randn(L);
    auto d = flt(x) + n;

    auto diag_load = 100.0 / mean(x * x);
    auto adapt = RlsFilterR(M, 0.99, diag_load);
    auto [y, e] = adapt(x, d);
    auto w = adapt.coeffs();
    ASSERT_LE(nmse(w, IR), 0.01);
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, LmsCmplx) {
    auto h = _get_bandpass_fir(32, 0.1, 0.2);
    int M = h.size();
    int L = 10000;
    auto flt = FirFilterC(h);
    arr_cmplx x = complex(randn(L), randn(L));
    arr_cmplx n = 0.01 * complex(randn(L), randn(L));
    arr_cmplx d = flt(x) + n;

    auto adapt = LmsFilterC(M, 0.5, LmsType::NLMS);
    auto [y, e] = adapt(x, d);
    auto w = adapt.coeffs();
    ASSERT_LE(nmse(w, h), 0.1);
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, RlsCmplx) {
    auto h = _get_bandpass_fir(32, 0.1, 0.2);
    int M = h.size();
    int L = 10000;
    auto flt = FirFilterC(h);
    arr_cmplx x = complex(randn(L), randn(L));
    arr_cmplx n = 0.01 * complex(randn(L), randn(L));
    arr_cmplx d = flt(x) + n;

    auto adapt = RlsFilterC(M, 0.98);
    auto [y, e] = adapt(x, d);
    auto w = adapt.coeffs();
    ASSERT_LE(nmse(w, h), 0.1);
}

//-------------------------------------------------------------------------------------------------
TEST(Fir1, Low) {
    auto tt = arange(8000);
    auto h = fir1(100, 0.1, FilterType::Low);
    const int N = h.size();
    ASSERT_EQ(N, 101);
    ASSERT_EQ(firtype(h), FirType::EvenSymm);
    auto x = sin(2 * pi * 0.2 * tt / 2);
    auto flt = FirFilter(h);
    auto y = flt(x);

    const arr_real in = x.slice(N, indexing::end);
    const arr_real out = y.slice(N, indexing::end);
    auto suppress = mag2db(rms(in) / rms(out));
    ASSERT_GE(suppress, 50);
}

TEST(Fir1, High) {
    auto tt = arange(8000);
    auto h = fir1(99, 0.1, FilterType::High);
    const int N = h.size();
    ASSERT_EQ(N, 101);
    ASSERT_EQ(firtype(h), FirType::EvenSymm);
    auto x = sin(2 * pi * 0.05 * tt / 2);
    auto flt = FirFilter(h);
    auto y = flt(x);

    const arr_real in = x.slice(N, indexing::end);
    const arr_real out = y.slice(N, indexing::end);
    auto suppress = mag2db(rms(in) / rms(out));
    ASSERT_GE(suppress, 50);
}

TEST(Fir1, Bandpass) {
    auto tt = arange(8000);
    auto h = fir1(99, 0.1, 0.2, FilterType::Bandpass);
    const int N = h.size();
    ASSERT_EQ(N, 100);
    ASSERT_EQ(firtype(h), FirType::OddSym);
    auto x = sin(2 * pi * 0.15 * tt / 2);
    auto flt = FirFilter(h);
    auto y = flt(x);

    const arr_real in = x.slice(N, indexing::end);
    const arr_real out = y.slice(N, indexing::end);
    auto suppress = mag2db(rms(in) / rms(out));
    ASSERT_NEAR(suppress, 0, 0.1);
}

TEST(Fir1, Bandstop) {
    auto tt = arange(8000);
    auto h = fir1(99, 0.1, 0.2, FilterType::Bandstop);
    const int N = h.size();
    ASSERT_EQ(N, 101);
    ASSERT_EQ(firtype(h), FirType::EvenSymm);
    auto x = sin(2 * pi * 0.15 * tt / 2);
    auto flt = FirFilter(h);
    auto y = flt(x);

    const arr_real in = x.slice(N, indexing::end);
    const arr_real out = y.slice(N, indexing::end);
    auto suppress = mag2db(rms(in) / rms(out));
    ASSERT_GE(suppress, 50);
}

//-------------------------------------------------------------------------------------------------
TEST(Fir1Type, Low) {
    {
        auto h = fir1(100, 0.1, FilterType::Low);
        ASSERT_EQ(h.size(), 101);
        ASSERT_EQ(int(firtype(h)), 1);
    }
    {
        auto h = fir1(101, 0.1, FilterType::Low);
        ASSERT_EQ(h.size(), 102);
        ASSERT_EQ(int(firtype(h)), 2);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(Fir1Type, High) {
    {
        auto h = fir1(100, 0.1, FilterType::High);
        ASSERT_EQ(h.size(), 101);
        ASSERT_EQ(int(firtype(h)), 1);
    }
    {
        auto h = fir1(101, 0.1, FilterType::High);
        ASSERT_EQ(h.size(), 103);
        ASSERT_EQ(int(firtype(h)), 1);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(Fir1Type, Bandpass) {
    {
        auto h = fir1(100, 0.1, 0.2, FilterType::Bandpass);
        ASSERT_EQ(h.size(), 101);
        ASSERT_EQ(int(firtype(h)), 1);
    }
    {
        auto h = fir1(101, 0.1, 0.2, FilterType::Bandpass);
        ASSERT_EQ(h.size(), 102);
        ASSERT_EQ(int(firtype(h)), 2);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(Fir1Type, Bandstop) {
    {
        auto h = fir1(100, 0.1, 0.2, FilterType::Bandstop);
        ASSERT_EQ(h.size(), 101);
        ASSERT_EQ(int(firtype(h)), 1);
    }
    {
        auto h = fir1(101, 0.1, 0.2, FilterType::Bandstop);
        ASSERT_EQ(h.size(), 103);
        ASSERT_EQ(int(firtype(h)), 1);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, Firtype) {
    ASSERT_EQ(firtype({0, 1, 2, 3}), FirType::NonlinearPhase);
    ASSERT_EQ(firtype({0, 1, 2, 1, 0}), FirType::EvenSymm);
    ASSERT_EQ(firtype({0, 1, 2, 2, 1, 0}), FirType::OddSym);
    ASSERT_EQ(firtype({0, 1, 2, -1, 0}), FirType::NonlinearPhase);
    ASSERT_EQ(firtype({0, 1, 0, -1, 0}), FirType::EvenAntiSym);
    ASSERT_EQ(firtype({0, 1, 2, -2, -1, 0}), FirType::OddAntiSym);
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, MovingAverage) {
    auto x = randn(10000);
    const int n = 100;

    FirFilterR fir_flt(ones(n) / n);
    const auto y1 = fir_flt.process(x);

    MAFilterR ma_flt(n);
    const auto y2 = ma_flt.process(x);

    ASSERT_EQ_ARR_REAL(y1, y2);
}