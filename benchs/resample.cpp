#include "dsplib/resample.h"
#include <benchmark/benchmark.h>

#include <dsplib.h>

static void BM_Decimator(benchmark::State& state) {
    const int fs = 16000;
    auto rsmp = dsplib::FIRDecimator(4);
    auto x = dsplib::randn(fs);
    for (auto _ : state) {
        auto y = rsmp.process(x);
    }
}

static void BM_Interpolator(benchmark::State& state) {
    const int fs = 16000;
    auto rsmp = dsplib::FIRInterpolator(4);
    auto x = dsplib::randn(fs);
    for (auto _ : state) {
        auto y = rsmp.process(x);
    }
}

static void BM_RateConverter(benchmark::State& state) {
    const int fs = 16000;
    auto rsmp = dsplib::FIRResampler(44100, fs);
    auto x = dsplib::randn(fs);
    for (auto _ : state) {
        auto y = rsmp.process(x);
    }
}

BENCHMARK(BM_Decimator)->Unit(benchmark::kMicrosecond)->MinTime(10);
BENCHMARK(BM_Interpolator)->Unit(benchmark::kMicrosecond)->MinTime(10);
BENCHMARK(BM_RateConverter)->Unit(benchmark::kMicrosecond)->MinTime(10);
