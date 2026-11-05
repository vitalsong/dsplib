#include "tests_common.h"
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
TEST(FirTest, CmplxImpulseResponse) {
    const arr_cmplx h = {1.0 + 2.0i, -0.5 + 0.25i, 0.75 - 1.5i};
    auto fir = FirFilterC(h);
    auto fft_fir = FftFilterC(h);
    auto x = complex(zeros(fft_fir.block_size()));
    x[0] = 1;
    arr_cmplx expected(x.size());
    expected.slice(0, h.size()) = h;

    ASSERT_EQ_ARR_CMPLX(fir(x), expected);
    ASSERT_EQ_ARR_CMPLX(fft_fir(x), expected);
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, FftEqFir) {
    for (int nh = 1; nh <= 128; ++nh) {
        const auto ir_coeff = fir1(nh, 0.1);
        auto flt1 = FftFilter(ir_coeff);
        auto flt2 = FirFilter(ir_coeff);
        auto in = randn(10000);
        auto x1 = flt1(in);
        x1 |= flt1.flush();
        auto x2 = flt2(in);
        ASSERT_EQ(x1.size(), in.size());
        ASSERT_EQ_ARR_REAL(x1, x2);
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
    x1 |= flt1.flush();
    auto x2 = flt2(in);
    ASSERT_EQ(x1.size(), in.size());
    ASSERT_EQ_ARR_CMPLX(x1, x2);
}

namespace {

template<typename T>
void check_fft_filter_chunks(const base_array<T>& h, const base_array<T>& x, bool flush_each) {
    FftFilter<T> fft_filter(h);
    FirFilter<T> fir_filter(h);
    const auto expected = fir_filter(x);
    EXPECT_EQ(fft_filter.flush().size(), 0);
    base_array<T> result;
    int offset = 0;
    int pending = 0;
    // Include calls shorter than the overlap, and calls spanning several FFT blocks.
    for (int count : {1, 2, fft_filter.block_size() - 1, 0,
                      fft_filter.block_size() + 3, 1, 3 * fft_filter.block_size()}) {
        base_array<T> actual;
        if (count == 0) {
            actual = fft_filter(base_array<T>{});
        } else {
            actual = fft_filter(x.slice(offset, offset + count));
        }
        const int nr = (pending + count) / fft_filter.block_size() * fft_filter.block_size();
        pending = (pending + count) % fft_filter.block_size();
        ASSERT_EQ(actual.size(), nr);
        result |= actual;
        offset += count;
        if (flush_each) {
            const auto remaining = fft_filter.flush();
            ASSERT_EQ(remaining.size(), pending);
            result |= remaining;
            pending = 0;
            EXPECT_EQ(fft_filter.flush().size(), 0);
        }
    }
    const auto remaining = fft_filter.flush();
    ASSERT_EQ(remaining.size(), pending);
    result |= remaining;
    ASSERT_EQ(result.size(), x.size());
    ASSERT_EQ_ARR_REAL(abs(result - expected), zeros(x.size()));
    EXPECT_EQ(fft_filter.flush().size(), 0);
    // Flushing buffered input must preserve the FIR state for the next call.
    const base_array<T> tail(h.size() + 2);
    auto actual_tail = fft_filter(tail);
    actual_tail |= fft_filter.flush();
    ASSERT_EQ_ARR_REAL(abs(actual_tail - fir_filter(tail)), zeros(tail.size()));
}

}   // namespace

TEST(FirTest, FftFirChunks) {
    for (const auto& h : std::vector<arr_real>{{0.2, -0.3, 0.5, 0.1}, fir1(100, 0.1)}) {
        FftFilterR filter(h);
        const int size = 5 * filter.block_size() + 6;
        const auto t = arange(size);
        for (bool flush_each : {false, true}) {
            check_fft_filter_chunks(h, sin(0.17 * t) + 0.3 * cos(0.41 * t), flush_each);
        }
    }
}

TEST(FirTest, CmplxFftFirChunks) {
    const arr_cmplx h = {1 + 2i, -0.5 + 0.25i, 0.75 - 1.5i};
    FftFilterC filter(h);
    const int size = 5 * filter.block_size() + 6;
    const auto t = arange(size);
    for (bool flush_each : {false, true}) {
        check_fft_filter_chunks(h, expj(0.17 * t) + 0.3 * expj(-0.41 * t), flush_each);
    }
}

TEST(FirTest, FftFirSingleTap) {
    const arr_real h = {2};
    const arr_real x = {1, 2, 3};
    FftFilterR real_filter(h);
    for (int i = 0; i < 2; ++i) {
        auto y = real_filter(x);
        ASSERT_EQ(y.size(), real_filter.block_size());
        y |= real_filter.flush();
        ASSERT_EQ_ARR_REAL(y, x * 2);
        EXPECT_EQ(real_filter.flush().size(), 0);
    }
    EXPECT_EQ(real_filter(arr_real{}).size(), 0);

    const arr_cmplx hc = {2 + 1i};
    const arr_cmplx xc = {1 + 1i, 2 - 1i, 3};
    FftFilterC complex_filter(hc);
    for (int i = 0; i < 2; ++i) {
        auto y = complex_filter(xc);
        ASSERT_EQ(y.size(), complex_filter.block_size());
        y |= complex_filter.flush();
        ASSERT_EQ_ARR_CMPLX(y, xc * hc[0]);
        EXPECT_EQ(complex_filter.flush().size(), 0);
    }
    EXPECT_EQ(complex_filter(arr_cmplx{}).size(), 0);
}

//-------------------------------------------------------------------------------------------------
static arr_cmplx _get_bandpass_fir(int len, double f0, double f1) {
    auto H = complex(zeros(len));
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
    const real_t noise_std = 0.01;
    const real_t mu = 0.5;
    auto h = _get_bandpass_fir(32, 0.1, 0.2);
    int M = h.size();
    int L = 10000;
    auto flt = FirFilterC(h);
    arr_cmplx x = complex(randn(L), randn(L));
    arr_cmplx n = noise_std * complex(randn(L), randn(L));
    arr_cmplx d = flt(x) + n;

    auto adapt = LmsFilterC(M, mu, LmsType::NLMS);
    auto [y, e] = adapt(x, d);
    auto w = adapt.coeffs();
    // Approximate coefficient MSE: (noise power / input power) * mu / ((2-mu) * M).
    // Allow for finite-sample effects and random noise fluctuations.
    const real_t max_mse = 10 * noise_std * noise_std * mu / ((2 - mu) * M);
    ASSERT_LE(mse(w, h), max_mse);
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, RlsCmplx) {
    const real_t noise_std = 0.01;
    const real_t mu = 0.98;
    auto h = _get_bandpass_fir(32, 0.1, 0.2);
    int M = h.size();
    int L = 10000;
    auto flt = FirFilterC(h);
    arr_cmplx x = complex(randn(L), randn(L));
    arr_cmplx n = noise_std * complex(randn(L), randn(L));
    arr_cmplx d = flt(x) + n;

    auto adapt = RlsFilterC(M, mu);
    auto [y, e] = adapt(x, d);
    auto w = adapt.coeffs();
    // Approximate coefficient MSE: (noise power / input power) * (1-mu)/(1+mu).
    // Allow for finite-sample covariance and random noise fluctuations.
    const real_t max_mse = 10 * noise_std * noise_std * (1 - mu) / (1 + mu);
    ASSERT_LE(mse(w, h), max_mse);
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
    ASSERT_EQ(firtype(arr_real{0, 1, 2, 3}), FirType::NonlinearPhase);
    ASSERT_EQ(firtype(arr_real{0, 1, 2, 1, 0}), FirType::EvenSymm);
    ASSERT_EQ(firtype(arr_real{0, 1, 2, 2, 1, 0}), FirType::OddSym);
    ASSERT_EQ(firtype(arr_real{0, 1, 2, -1, 0}), FirType::NonlinearPhase);
    ASSERT_EQ(firtype(arr_real{0, 1, 0, -1, 0}), FirType::EvenAntiSym);
    ASSERT_EQ(firtype(arr_real{0, 1, 2, -2, -1, 0}), FirType::OddAntiSym);
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, MAFilter) {
    //real filter(h, 1, x) == mafilter(x)
    {
        auto x = randn(10000);
        const int w = 50;
        auto h = ones(w) / w;
        FirFilterR fir_flt(h);
        const auto y1 = fir_flt.process(x);
        MAFilterR ma_flt(w);
        const auto y2 = ma_flt.process(x);
        ASSERT_EQ_ARR_REAL(y1, y2);
    }

    //cmplx filter(h, 1, x) == mafilter(x)
    {
        auto x = hilbert(randn(10000));
        const int w = 63;
        auto h = complex(ones(w) / w);
        FirFilterC fir_flt(h);
        const auto y1 = fir_flt.process(x);
        MAFilterC ma_flt(w);
        const auto y2 = ma_flt.process(x);
        ASSERT_EQ_ARR_CMPLX(y1, y2);
    }

    //real
    {
        auto in = randn(1000);
        MAFilterR ma_flt(1);
        const auto out = ma_flt.process(in);
        ASSERT_EQ_ARR_REAL(in, out);
    }

    //cmplx
    {
        auto in = hilbert(randn(1000));
        MAFilterC ma_flt(1);
        const auto out = ma_flt.process(in);
        ASSERT_EQ_ARR_CMPLX(in, out);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(FirTest, MAFilterStress) {
    int num_tests = 1000;
    while (--num_tests != 0) {
        auto x = randn(10000);
        const int w = randi({2, 500});
        auto h = ones(w) / w;
        FirFilterR fir_flt(h);
        const auto y1 = fir_flt.process(x);
        MAFilterR ma_flt(w);
        const auto y2 = ma_flt.process(x);
        ASSERT_EQ_ARR_REAL(y1, y2);
    }
}
