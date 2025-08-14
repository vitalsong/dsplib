#include <benchmark/benchmark.h>
#include <dsplib.h>

static void BM_COMPRESSOR(benchmark::State& state) {
    const int fs = 16000;
    dsplib::arr_real x = dsplib::randn(fs);
    auto comp = dsplib::Compressor(fs);
    for (auto _ : state) {
        auto y = comp.process(x);
        x[0] += 1e-5;
        benchmark::DoNotOptimize(y);
    }
}

static void BM_LIMITER(benchmark::State& state) {
    const int fs = 16000;
    dsplib::arr_real x = dsplib::randn(fs);
    auto limt = dsplib::Limiter(fs);
    for (auto _ : state) {
        auto y = limt.process(x);
        x[0] += 1e-5;
        benchmark::DoNotOptimize(y);
    }
}

static void BM_AGC(benchmark::State& state) {
    const int fs = 16000;
    dsplib::arr_real x = dsplib::randn(fs);
    auto agc = dsplib::Agc();
    for (auto _ : state) {
        auto y = agc.process(x);
        x[0] += 1e-5;
        benchmark::DoNotOptimize(y);
    }
}

BENCHMARK(BM_COMPRESSOR)->Unit(benchmark::kMicrosecond)->MinTime(5);
BENCHMARK(BM_LIMITER)->Unit(benchmark::kMicrosecond)->MinTime(5);
BENCHMARK(BM_AGC)->Unit(benchmark::kMicrosecond)->MinTime(5);
