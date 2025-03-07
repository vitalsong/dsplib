#include <benchmark/benchmark.h>

#include "pocketfft_hdronly.h"

#include <cstdint>
#include <dsplib.h>
#include <vector>

constexpr int MIN_TIME = 5;

#ifdef KISSFFT_SUPPORT

#include "kiss_fft.h"

static void BM_KISSFFT(benchmark::State& state) {
    const int n = state.range(0);
    kiss_fft_cfg cfg = kiss_fft_alloc(n, false, 0, 0);
    std::vector<kiss_fft_cpx> in(n);
    std::vector<kiss_fft_cpx> out(n);
    for (auto _ : state) {
        in[0].r += 1e-5;
        kiss_fft(cfg, in.data(), out.data());
    }
    kiss_fft_free(cfg);
}

BENCHMARK(BM_KISSFFT)
  ->Arg(1024)
  ->Arg(1331)
  ->Arg(512 * 3)
  ->Arg(64 * 31)
  ->Arg(2048)
  ->Arg(4096)
  ->Arg(8192)
  ->Arg(11200)
  ->Arg(11202)
  ->Arg(16384)
  ->MinTime(MIN_TIME)
  ->Unit(benchmark::kMicrosecond);

#endif

#ifdef FFTW_SUPPORT

#include "fftw3.h"

static void BM_FFTW3_FLOAT(benchmark::State& state) {
    const int n = state.range(0);
    auto in = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * n);
    auto out = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * n);
    fftwf_plan plan = fftwf_plan_dft_1d(n, in, out, FFTW_FORWARD, FFTW_MEASURE);
    fftwf_execute(plan);

    for (auto _ : state) {
        in[0][0] += 1e-5;
        fftwf_execute(plan);
    }

    fftwf_destroy_plan(plan);
    fftwf_free(in);
    fftwf_free(out);
}

static void BM_FFTW3_DOUBLE(benchmark::State& state) {
    const int n = state.range(0);
    auto in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * n);
    auto out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * n);
    fftw_plan plan = fftw_plan_dft_1d(n, in, out, FFTW_FORWARD, FFTW_MEASURE);
    fftw_execute(plan);

    for (auto _ : state) {
        in[0][0] += 1e-5;
        fftw_execute(plan);
    }

    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);
}

BENCHMARK(BM_FFTW3_DOUBLE)
  ->Arg(1024)
  ->Arg(1331)
  ->Arg(512 * 3)
  ->Arg(64 * 31)
  ->Arg(2048)
  ->Arg(4096)
  ->Arg(8192)
  ->Arg(11200)
  ->Arg(11202)
  ->Arg(16384)
  ->MinTime(MIN_TIME)
  ->Unit(benchmark::kMicrosecond);

#endif

static void BM_FFT_POCKETFFT(benchmark::State& state) {
    const uint32_t n = state.range(0);

    constexpr std::ptrdiff_t sz = sizeof(dsplib::cmplx_t);
    const pocketfft::stride_t stride{static_cast<int32_t>(sz * n), sz};

    auto x = complex(dsplib::randn(n));
    auto y = dsplib::complex(dsplib::zeros(n));

    auto tx = (std::complex<dsplib::real_t>*)x.data();
    auto ty = (std::complex<dsplib::real_t>*)y.data();

    for (auto _ : state) {
        //its fucking ugly
        pocketfft::c2c<dsplib::real_t>({1, n}, stride, stride, {0, 1}, pocketfft::FORWARD, tx, ty, 1.0);
        x[0].re += 1e-5;
    }
}

BENCHMARK(BM_FFT_POCKETFFT)
  ->Arg(1024)
  ->Arg(1331)
  ->Arg(512 * 3)
  ->Arg(64 * 31)
  ->Arg(2048)
  ->Arg(4096)
  ->Arg(8192)
  ->Arg(11200)
  ->Arg(11202)
  ->Arg(16384)
  ->MinTime(MIN_TIME)
  ->Unit(benchmark::kMicrosecond);

static void BM_FFT_DSPLIB(benchmark::State& state) {
    const int n = state.range(0);
    auto x = complex(dsplib::randn(n));
    dsplib::arr_cmplx y = dsplib::fft(x);   ///< update cache
    for (auto _ : state) {
        y = dsplib::fft(x);
        x[0] += 1e-5;
    }
}

BENCHMARK(BM_FFT_DSPLIB)
  ->Arg(1024)
  ->Arg(1331)
  ->Arg(512 * 3)
  ->Arg(64 * 31)
  ->Arg(2048)
  ->Arg(4096)
  ->Arg(8192)
  ->Arg(11200)
  ->Arg(11202)
  ->Arg(16384)
  ->MinTime(MIN_TIME)
  ->Unit(benchmark::kMicrosecond);
