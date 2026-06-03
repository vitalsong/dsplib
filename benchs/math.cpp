#include <benchmark/benchmark.h>

#include <dsplib.h>

static void BM_DOT_REAL(benchmark::State& state) {
    const int n = state.range(0);
    auto x1 = dsplib::randn(n);
    auto x2 = dsplib::randn(n);
    for (auto _ : state) {
        auto y = dsplib::dot(x1, x2);
        benchmark::DoNotOptimize(y);
    }
}

static void BM_DOT_CMPLX(benchmark::State& state) {
    const int n = state.range(0);
    auto x1 = dsplib::complex(dsplib::randn(n), dsplib::randn(n));
    auto x2 = dsplib::complex(dsplib::randn(n), dsplib::randn(n));
    for (auto _ : state) {
        auto y = dsplib::dot(x1, x2);
        benchmark::DoNotOptimize(y);
    }
}

BENCHMARK(BM_DOT_REAL)->Arg(128)->Arg(512)->Arg(2048)->Arg(8192)->MinTime(1)->Unit(benchmark::kNanosecond);

BENCHMARK(BM_DOT_CMPLX)->Arg(128)->Arg(512)->Arg(2048)->Arg(8192)->MinTime(1)->Unit(benchmark::kNanosecond);