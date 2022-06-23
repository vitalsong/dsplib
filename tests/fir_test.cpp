#include "tests_common.h"
#include <math.h>

//-------------------------------------------------------------------------------------------------
const dsplib::arr_real impulses = {
  0.0009696959912217, 0.002337889165401, -5.132250197644e-05, -0.01078532572765, -0.02511021325159,  -0.02605521284747,
  -0.001729717261329, 0.03055533348918,  0.02997831233377,    -0.02035756228145, -0.07197861086446,  -0.03952554411153,
  0.1086584225047,    0.2928228574697,   0.3766576194224,     0.2928228574697,   0.1086584225047,    -0.03952554411153,
  -0.07197861086446,  -0.02035756228145, 0.02997831233377,    0.03055533348918,  -0.001729717261329, -0.02605521284747,
  -0.02511021325159,  -0.01078532572765, -5.132250197644e-05, 0.002337889165401, 0.0009696959912217};

//-------------------------------------------------------------------------------------------------
TEST(FirTest, FirAndOne) {
    using namespace dsplib;
    auto flt = fir(impulses);
    auto x = zeros(impulses.size());
    x[0] = 1;
    auto y = flt(x);
    ASSERT_EQ_ARR_REAL(impulses, y);
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, FftFirAndOne) {
    using namespace dsplib;
    auto flt = fir_fft(impulses);
    auto x = zeros(2 * impulses.size());
    x[0] = 1;
    arr_real y = flt(x).slice(0, impulses.size());
    ASSERT_EQ_ARR_REAL(impulses, y);
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, FftEqFir) {
    using namespace dsplib;
    auto flt1 = fir_fft(impulses);
    auto flt2 = fir(impulses);
    auto in = randn(10000);
    auto x1 = flt1(in);
    auto x2 = flt2(in);
    int n = min(x1.size(), x2.size());
    arr_real y1 = x1.slice(0, n);
    arr_real y2 = x2.slice(0, n);
    ASSERT_NE(n, 0);
    ASSERT_EQ_ARR_REAL(y1, y2);
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, NonSymIR) {
    using namespace dsplib;
    auto h = randn(103);
    auto flt = fir(h);
    auto x = zeros(h.size());
    x[0] = 1;
    auto y = flt(x);
    ASSERT_EQ_ARR_REAL(h, y);
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, CmplxFftFir) {
    using namespace dsplib;
    auto h = randn(100) + randn(100) * 1i;
    auto flt1 = fir_fft(h);
    auto flt2 = fir_cmplx(h);
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
static dsplib::arr_cmplx _get_bandpass_fir(int len, double f0, double f1) {
    dsplib::arr_cmplx H = dsplib::zeros(len);
    auto t0 = int(f0 * len);
    auto t1 = int(f1 * len);
    H.slice(t0, t1) = 1;
    dsplib::arr_cmplx h = ifft(H);
    h = dsplib::arr_cmplx(h.slice(len / 2, len)) | dsplib::arr_cmplx(h.slice(0, len / 2));
    h *= dsplib::window::hamming(len);
    return h;
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, Lms) {
    using namespace dsplib;
    int M = impulses.size();
    int L = 1000;
    auto flt = fir(impulses);
    auto x = randn(L);
    auto n = 0.01 * randn(L);
    auto d = flt(x) + n;

    auto mu_max = 2 / ((M + 1) * mean(x * x));
    auto mu = 0.05 * mu_max;
    auto adapt = lms(M, mu);
    auto [y, e] = adapt(x, d);
    auto w = adapt.coeffs();
    ASSERT_LE(nmse(w, impulses), 1e-3);
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, Rls) {
    int M = impulses.size();
    int L = 1000;
    auto flt = dsplib::fir(impulses);
    auto x = dsplib::randn(L);
    auto n = 0.01 * dsplib::randn(L);
    auto d = flt(x) + n;

    auto diag_load = 100.0 / dsplib::mean(x * x);
    auto adapt = dsplib::rls(M, 0.99, diag_load);
    auto [y, e] = adapt(x, d);
    auto w = adapt.coeffs();
    ASSERT_LE(nmse(w, impulses), 0.01);
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, LmsCmplx) {
    using namespace dsplib;
    auto h = _get_bandpass_fir(32, 0.1, 0.2);
    int M = h.size();
    int L = 10000;
    auto flt = fir_cmplx(h);
    arr_cmplx x = complex(randn(L), randn(L));
    arr_cmplx n = 0.01 * complex(randn(L), randn(L));
    arr_cmplx d = flt(x) + n;

    auto adapt = clms(M, 0.5, lms_type::NLMS);
    auto [y, e] = adapt(x, d);
    auto w = adapt.coeffs();
    ASSERT_LE(nmse(w, h), 0.1);
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, RlsCmplx) {
    using namespace dsplib;
    auto h = _get_bandpass_fir(32, 0.1, 0.2);
    int M = h.size();
    int L = 10000;
    auto flt = fir_cmplx(h);
    arr_cmplx x = complex(randn(L), randn(L));
    arr_cmplx n = 0.01 * complex(randn(L), randn(L));
    arr_cmplx d = flt(x) + n;

    auto adapt = crls(M, 0.98);
    auto [y, e] = adapt(x, d);
    auto w = adapt.coeffs();
    ASSERT_LE(nmse(w, h), 0.1);
}