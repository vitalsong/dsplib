#include <benchmark/benchmark.h>

#include <dsplib.h>

static void BM_IFFT_DSPLIB(benchmark::State& state) {
    const int n = state.range(0);
    dsplib::arr_cmplx x = dsplib::randn(n);
    auto y = dsplib::fft(x);
    for (auto _ : state) {
        y[0] += 1e-10;
        x = dsplib::ifft(y);
    }
}

static void BM_IRFFT_DSPLIB(benchmark::State& state) {
    const int n = state.range(0);
    dsplib::arr_real x = dsplib::randn(n);
    auto y = dsplib::fft(x);
    for (auto _ : state) {
        y[0] += 1e-10;
        x = dsplib::irfft(y);
    }
}

BENCHMARK(BM_IFFT_DSPLIB)
  ->Arg(1024)
  ->Arg(2048)
  ->Arg(4096)
  ->Arg(8192)
  ->Arg(11200)
  ->Arg(11202)
  ->Arg(16384)
  ->MinTime(1)
  ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_IRFFT_DSPLIB)
  ->Arg(1024)
  ->Arg(2048)
  ->Arg(4096)
  ->Arg(8192)
  ->Arg(11200)
  ->Arg(11202)
  ->Arg(16384)
  ->MinTime(1)
  ->Unit(benchmark::kMicrosecond);