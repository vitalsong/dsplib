#include <benchmark/benchmark.h>
#include <dsplib.h>

static void BM_SLICE_ORDERED(benchmark::State& state) {
    const int M = state.range(0);
    dsplib::arr_real x1 = dsplib::randn(M);
    const dsplib::arr_real x2 = dsplib::randn(M);
    for (auto _ : state) {
        x1.slice(0, M) = x2.slice(0, M);
    }
}

static void BM_SLICE_REVERSE(benchmark::State& state) {
    const int M = state.range(0);
    dsplib::arr_real x1 = dsplib::randn(M);
    for (auto _ : state) {
        x1.slice(0, 2) = x1.slice(2, 0, -1);
    }
}

static void BM_SLICE_UNORDERED(benchmark::State& state) {
    const int M = state.range(0);
    dsplib::arr_real x1 = dsplib::randn(M);
    const dsplib::arr_real x2 = dsplib::randn(M);
    for (auto _ : state) {
        x1.slice(0, M, 2) = x2.slice(0, M, 2);
    }
}

BENCHMARK(BM_SLICE_ORDERED)->Arg(8192)->Arg(32767)->Unit(benchmark::kMicrosecond)->MinTime(3);
BENCHMARK(BM_SLICE_UNORDERED)->Arg(8192)->Arg(32767)->Unit(benchmark::kMicrosecond)->MinTime(3);
BENCHMARK(BM_SLICE_REVERSE)->Arg(8192)->Arg(32767)->Unit(benchmark::kMicrosecond)->MinTime(3);
