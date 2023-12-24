# DSPLIB
![testing](https://github.com/vitalsong/dsplib/actions/workflows/android-ndk.yml/badge.svg)
![testing](https://github.com/vitalsong/dsplib/actions/workflows/linux.yml/badge.svg)
![testing](https://github.com/vitalsong/dsplib/actions/workflows/windows.yml/badge.svg)

C++ DSP library for MATLAB similar programming.

This is a library for those who hate this kind of code:

```cpp
std::transform(x.begin(), x.end(), x.begin(), [](auto& v){
    return v * 0.3;
});

auto r = std::vector<double>(x1.size());
for (int i=0; i < r.size(); ++i) {
    r[i] = x1[i] * x2[i];
}

auto p = fftw_plan_dft_1d(N, x, spec, FFTW_FORWARD, FFTW_ESTIMATE);
fftw_execute(p);
fftw_destroy_plan(p);
```

and who likes this:

```cpp
x *= 0.3;
r = x1 * x2;
auto spec = dsplib::fft(x);
```

## Usage

| dsplib      | matlab      | numpy       |
| ----------- | ----------- | ----------- |
| x * x       | x .* x      | x * x |
| zeros(20)   | zeros(20, 1) | zeros(20) |
| x.slice(0,10) = 1 | x(1:10) = 1 | x[0:10] = 1 |
| x.slice(2,end) = 1 | x(3:end) = 1 | x[2:] = 1 |
| x.slice(2, 4) = {1, 2} | x(3:4) = [1, 2] | x[2:4] = [1, 2] |
| x.slice(0, -1) | x(1:end-1) | x[0:-1] |

> Only 1-D arrays with element-wise operations are currently supported.

### Scalar operations:
```cpp
dsplib::real_t v1;
dsplib::cmplx_t v2;
v1 = 10;
v2 = v1;
v2 = 10-10i;
v2 = {10, -10};
v2.re = 10;
v2.im = -10;
v2 = std::complex<double>(10, -10);
```

### Vector operations:
```cpp
using namespace dsplib;
arr_real x1 = {0, 1, 2, 3, 4};
arr_cmplx x2 = {1i, 1+2i, 4, -5-5i};
x2.slice(2, 4) = {1i, 2i};
arr_real y1 = x1 * x1;
arr_real y2 = x1 * 1000;
arr_cmplx y3 = x1 * x2;
arr_cmplx y4 = x2 * 1000;
arr_cmplx y5 = x2.slice(0, 2);
arr_cmplx y6 = x1 * 2i;
```

### Slicing
The behavior of slices is as close as possible to numpy. Except for cases with invalid indexes, in which case numpy does not throw an exception.
```cpp
arr_real x = {0, 1, 2, 3, 4, 5, 6};
x.slice(0, 2) ///{0, 1}
x.slice(2, -1) ///{2, 3, 4, 5}
x.slice(-1, 0, -1) ///{6, 5, 4, 3, 2, 1}
x.slice(-1, 0) ///OUT_OF_RANGE, but numpy returns []
x.slice(0, -1, -1) ///OUT_OF_RANGE, but numpy returns []
x.slice(-8, 7) ///OUT_OF_RANGE, but numpy returns [0 1 2 3 4 5 6]
```

### Fast Fourier Transform:
The FFT/IFFT calculation table is cached on first run. To eliminate this behavior, you can use the FftPlan object.
```cpp
arr_real x = randn(500);
arr_cmplx y1 = fft(x);  //500
arr_cmplx y2 = fft(x, 1024); //1024
```

### Inverse Fast Fourier Transform:
```cpp
arr_cmplx x = 1i * zeros(512);
x[10] = 1;
arr_cmplx y = ifft(x);
```

### FIR filter:
```cpp
const auto h = fir1(100, 0.1, FilterType::Low);
auto flt = FftFilter(h);
arr_real x = randn(10000);
arr_real y = flt(x);
```

### Hilbert filter:
```cpp
auto flt = HilbertFilter();
arr_real x = randn(10000);
arr_cmplx y1 = flt(x);
//or
arr_cmplx y2 = hilbert(x);
```

### Add White Gaussian Noise:
```cpp
arr_real x = randn(10000);
arr_real y = awgn(x, 10);   //snr=10dB
```

### Cross-correlation:
```cpp
arr_real x1 = randn(500);
arr_real x2 = awgn(x1, 10);
arr_real y = xcorr(x1, x2);
```

### Delay estimation:
```cpp
arr_real x1 = randn(500);
arr_real x2 = delayseq(x1, 100);
auto d1 = finddelay(x1, x2);    ///d1=100
auto [d2, _] = gccphat(x2, x1);   ///d2=100.0
```

### Spectrum Analyze:
```cpp
const int n = 1024;
arr_real x = randn(n);
x *= window::hann(n);
arr_cmplx y = fft(x) / (n/2);
y = y.slice(0, n/2+1);
auto z = pow2db(abs2(y));   //20*log10(..)
```

### FIR filter design:
```cpp
auto x = randn(10000);
auto h = fir1(99, 0.1, 0.2, FilterType::Bandstop);
auto flt = FirFilter(h);
auto y = flt(x);
```

### Adaptive filters:
```cpp
//simulate room impulse response
int M = 50;
auto rir = fir1(M-1, 0.1);
auto flt = FirFilter(rir);

int L = 100000;
auto x = randn(L);      //ref signal
auto n = 0.01 * randn(L);
auto d = flt(x) + n;    //mic signal

auto rls = RlsFilterR(M, 0.98, 1000);
auto [y, e] = rls(x, d);

ASSERT_LE(nmse(flt.coeffs(), rir), 0.01);
```

### Resampling:

To process signal in batches (for example, in real time), use modules `FIRDecimator`, `FIRInterpolator`, `FIRRateConverter` or `FIRResampler`. The output signal will be delayed, but there will be no gap between frames.

To process the entire signal (for example, from a file), use function `resample(x, p, q)`. The processing result will be aligned.

```cpp
//44100 -> 16000
auto rsmp = dsplib::FIRResampler(16000, 44100);
auto out = rsmp.process(in);
//or
auto rsmp = dsplib::FIRRateConverter(160, 441);
auto out = rsmp.process(in);
//or
auto out = dsplib::resample(in, 16000, 44100);
//or
auto out = dsplib::resample(in, 160, 441);
```

```cpp
//32000 -> 16000
auto decim = dsplib::FIRDecimator(2);
auto out = decim.process(in);
//or
auto out = dsplib::resample(in, 1, 2);
//or
auto out = dsplib::resample(in, 16000, 32000);
```

```cpp
//16000 -> 32000
auto interp = dsplib::FIRInterpolator(2);
auto out = interp.process(in);
//or
auto out = dsplib::resample(in, 2, 1);
//or
auto out = dsplib::resample(in, 32000, 16000);
```

## Building

### Requires:
- CMake (>=3.10)
- C++ compiler for C++17 standard (gcc, clang, msvc. mingw)


### Build and install:

```sh
# set DSPLIB_USE_FLOAT32=ON to enable float base type (double by default)
# set DSPLIB_NO_EXCEPTIONS=ON to disable exceptions
# set BUILD_SHARED_LIBS=ON to build shared lib
cmake . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target=install
```

### Use CPM manager:

```cmake
CPMAddPackage(NAME dsplib
    GIT_REPOSITORY 
        "https://github.com/vitalsong/dsplib.git"
    VERSION 
        0.45.0
    OPTIONS
        "DSPLIB_USE_FLOAT32 OFF"
        "DSPLIB_NO_EXCEPTIONS OFF"
    EXCLUDE_FROM_ALL ON
)
target_link_libraries(${PROJECT_NAME} dsplib)
```

## Performance

To build and run benchmarks:
```sh
cmake . -B build -DCMAKE_BUILD_TYPE=Release -DDSPLIB_BUILD_BENCHS=ON
cmake --build build
./build/benchs/dsplib-benchs
```

The implementation of a power-of-two size FFT is not much different from other popular implementations. For other cases, the `czt` algorithm is used, which, however, loses in terms of well-factorizable sizes.

> Use `FFT(N!=2^K)` only if you know what you are doing.

```
Run on (12 X 2600 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB
  L1 Instruction 32 KiB
  L2 Unified 256 KiB (x6)
  L3 Unified 12288 KiB
Load Average: 6.33, 7.60, 6.51
-----------------------------------------------------------------------------
Benchmark                                   Time             CPU   Iterations
-----------------------------------------------------------------------------
BM_FFT_KISS/1024/min_time:5.000          8.35 us         8.35 us       836698
BM_FFT_KISS/2048/min_time:5.000          22.6 us         22.6 us       309803
BM_FFT_KISS/4096/min_time:5.000          40.4 us         40.4 us       172859
BM_FFT_KISS/8192/min_time:5.000           113 us          113 us        62113
BM_FFT_KISS/11200/min_time:5.000          299 us          299 us        23374
BM_FFT_KISS/11202/min_time:5.000        42300 us        42295 us          166
BM_FFT_KISS/16384/min_time:5.000          228 us          228 us        30997
BM_FFTW3/1024/min_time:5.000             8.83 us         8.77 us      3352869
BM_FFTW3/2048/min_time:5.000              200 us          199 us       365305 (WTF?)
BM_FFTW3/4096/min_time:5.000             12.7 us         12.7 us       564812
BM_FFTW3/8192/min_time:5.000             30.4 us         30.3 us       228224
BM_FFTW3/11200/min_time:5.000            47.6 us         47.6 us       147614
BM_FFTW3/11202/min_time:5.000             285 us          284 us        23909
BM_FFTW3/16384/min_time:5.000            74.0 us         73.9 us        93175
BM_FFT_DSPLIB/1024/min_time:5.000        8.08 us         8.06 us       875500
BM_FFT_DSPLIB/2048/min_time:5.000        16.9 us         16.9 us       414268
BM_FFT_DSPLIB/4096/min_time:5.000        38.7 us         38.6 us       180155
BM_FFT_DSPLIB/8192/min_time:5.000        87.0 us         86.9 us        80394
BM_FFT_DSPLIB/11200/min_time:5.000       1390 us         1388 us         5120 (czt)
BM_FFT_DSPLIB/11202/min_time:5.000       1389 us         1387 us         4843 (czt)
BM_FFT_DSPLIB/16384/min_time:5.000        214 us          211 us        34804
```

## TODO:
- Use FFT factorization instead CZT for N != 2^K
- Select FFT backend type (fftw/ne10)
- Add matrix syntax support
- Add custom allocator for `base_array<T>` type
- Add audioread/audiowrite functions (optional libsndfile?)
- Add chain syntax like `fft(x)->abs2()->pow2db()`
- Use `const_span<T>` args