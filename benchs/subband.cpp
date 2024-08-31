#include <benchmark/benchmark.h>
#include <dsplib.h>

#include "dsplib/subband.h"

constexpr int M = 4;
constexpr int N = 1024;
constexpr int D = 2;

static void BM_Channelizer(benchmark::State& state) {
    auto chan = dsplib::Channelizer(N, D, M);
    auto x = dsplib::randn(N / D);
    for (auto _ : state) {
        auto y = chan.process(x);
        benchmark::DoNotOptimize(y);
    }
}

static void BM_ChannelSynthesizer(benchmark::State& state) {
    auto chan = dsplib::ChannelSynthesizer(N, D, M);
    dsplib::arr_cmplx x = dsplib::randn(N);
    for (auto _ : state) {
        auto y = chan.process(x);
        benchmark::DoNotOptimize(y);
    }
}

BENCHMARK(BM_Channelizer)->Unit(benchmark::kMicrosecond)->MinTime(5);
BENCHMARK(BM_ChannelSynthesizer)->Unit(benchmark::kMicrosecond)->MinTime(5);