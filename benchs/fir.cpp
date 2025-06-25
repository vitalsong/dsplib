#include <benchmark/benchmark.h>

#include <dsplib.h>

constexpr int SIGNAL_LEN = 16000;

static void BM_FIR_REAL(benchmark::State& state) {
    const int nh = state.range(0);
    const auto h = dsplib::randn(nh);
    auto flt = dsplib::FirFilterR(h);
    const auto x = dsplib::randn(SIGNAL_LEN);
    for (auto _ : state) {
        auto y = flt(x);
        benchmark::DoNotOptimize(y);
    }
}

static void BM_FIR_CMPLX(benchmark::State& state) {
    const int nh = state.range(0);
    const auto h = dsplib::complex(dsplib::randn(nh));
    auto flt = dsplib::FirFilterC(h);
    const auto x = dsplib::complex(dsplib::randn(SIGNAL_LEN));
    for (auto _ : state) {
        auto y = flt(x);
        benchmark::DoNotOptimize(y);
    }
}

static void BM_MAFLT_REAL(benchmark::State& state) {
    const int nh = state.range(0);
    auto flt = dsplib::MAFilterR(nh);
    const auto x = dsplib::randn(SIGNAL_LEN);
    for (auto _ : state) {
        auto y = flt(x);
        benchmark::DoNotOptimize(y);
    }
}

static void BM_FFTFLT_REAL(benchmark::State& state) {
    const int nh = state.range(0);
    const auto h = dsplib::randn(nh);
    auto flt = dsplib::FftFilter(h);
    const int siglen = SIGNAL_LEN / flt.block_size() * flt.block_size();
    const auto x = dsplib::randn(siglen);
    for (auto _ : state) {
        auto y = flt(x);
        benchmark::DoNotOptimize(y);
    }
}

BENCHMARK(BM_FIR_REAL)
  ->Arg(3)
  ->Arg(9)
  ->Arg(21)
  ->Arg(121)
  ->Arg(200)
  ->Arg(500)
  ->Arg(1000)
  ->MinTime(1)
  ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_FIR_CMPLX)
  ->Arg(3)
  ->Arg(9)
  ->Arg(21)
  ->Arg(121)
  ->Arg(200)
  ->Arg(500)
  ->Arg(1000)
  ->MinTime(1)
  ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_MAFLT_REAL)
  ->Arg(3)
  ->Arg(9)
  ->Arg(21)
  ->Arg(121)
  ->Arg(200)
  ->Arg(500)
  ->Arg(1000)
  ->MinTime(1)
  ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_FFTFLT_REAL)
  ->Arg(3)
  ->Arg(9)
  ->Arg(21)
  ->Arg(121)
  ->Arg(200)
  ->Arg(500)
  ->Arg(1000)
  ->MinTime(1)
  ->Unit(benchmark::kMicrosecond);