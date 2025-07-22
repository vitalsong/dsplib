#include <benchmark/benchmark.h>
#include <dsplib.h>

static std::pair<dsplib::arr_real, dsplib::arr_real> _prepare_frame(int M, int L) {
    const auto h = dsplib::randn(M);
    auto flt = dsplib::FirFilter(h);
    auto x = dsplib::randn(L);
    auto n = 0.01 * dsplib::randn(L);
    auto d = flt(x) + n;
    return {x, d};
}

static void BM_LMS(benchmark::State& state) {
    const int M = state.range(0);
    auto [x, d] = _prepare_frame(M, 512);
    auto mu_max = 2 / ((M + 1) * mean(x * x));
    auto mu = 0.05 * mu_max;
    auto af = dsplib::LmsFilterR(M, mu);
    for (auto _ : state) {
        auto [y, e] = af(x, d);
    }
}

static void BM_NLMS(benchmark::State& state) {
    const int M = state.range(0);
    auto [x, d] = _prepare_frame(M, 512);
    auto af = dsplib::LmsFilterR(M, 0.01, dsplib::LmsType::NLMS);
    for (auto _ : state) {
        auto [y, e] = af(x, d);
    }
}

static void BM_RLS(benchmark::State& state) {
    const int M = state.range(0);
    auto [x, d] = _prepare_frame(M, 512);
    auto af = dsplib::RlsFilterR(M);
    for (auto _ : state) {
        auto [y, e] = af(x, d);
    }
}

BENCHMARK(BM_LMS)->Arg(32)->Arg(64)->Unit(benchmark::kMicrosecond)->MinTime(3);
BENCHMARK(BM_NLMS)->Arg(32)->Arg(64)->Unit(benchmark::kMicrosecond)->MinTime(3);
BENCHMARK(BM_RLS)->Arg(32)->Arg(64)->Unit(benchmark::kMicrosecond)->MinTime(3);
