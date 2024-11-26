# DSPLIB
[![codecov](https://codecov.io/gh/vitalsong/dsplib/graph/badge.svg?token=8QBUTFWQI3)](https://codecov.io/gh/vitalsong/dsplib)
![testing](https://github.com/vitalsong/dsplib/actions/workflows/android-ndk.yml/badge.svg)
![testing](https://github.com/vitalsong/dsplib/actions/workflows/linux.yml/badge.svg)
![testing](https://github.com/vitalsong/dsplib/actions/workflows/windows.yml/badge.svg)

C++ DSP library for MATLAB similar programming.

This is a library for those who hate this kind of code:

```cpp
std::transform(x.begin(), x.end(), x.begin(), [](auto& v){
    return v * 0.3;
});

auto power = std::accumulate(x.begin() + lb, x.begin() + rb, 0.0, [](double accum, const std::complex<double>& v) {
    return accum + (v.real() * v.real() + v.imag() * v.imag());
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
using namespace dsplib;
x *= 0.3;
auto power = sum(abs2(*x.slice(lb, rb)));
auto r = x1 * x2;
auto spec = fft(x);
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
- C++ compiler for C++17 standard (gcc, clang, msvc, mingw)


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

The implementation of non-power-of-two FFT is based on the general factorization algorithm. It is usually slower, but not critical. 

For prime and semi-prime numbers, the czt algorithm is used, which can be significantly slower (but not as slow as regular DFT).

> Use `FFT(N!=2^K)` only if you know what you are doing.

```
Run on (20 X 5100 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 2048 KiB (x10)
  L3 Unified 24576 KiB (x1)
-------------------------------------------------------------------------------
Benchmark                                     Time             CPU   Iterations
-------------------------------------------------------------------------------
BM_FFT_DSPLIB/1024/min_time:5.000             4.32 us         4.32 us      1612493
BM_FFT_DSPLIB/1331/min_time:5.000             37.6 us         37.6 us       184823
BM_FFT_DSPLIB/1536/min_time:5.000             12.4 us         12.4 us       542987
BM_FFT_DSPLIB/1984/min_time:5.000             60.3 us         60.3 us       112019
BM_FFT_DSPLIB/2048/min_time:5.000             9.91 us         9.91 us       702093
BM_FFT_DSPLIB/4096/min_time:5.000             22.1 us         22.1 us       318985
BM_FFT_DSPLIB/8192/min_time:5.000             46.5 us         46.5 us       150514
BM_FFT_DSPLIB/11200/min_time:5.000             264 us          264 us        26518
BM_FFT_DSPLIB/11202/min_time:5.000             481 us          481 us        14543
BM_FFT_DSPLIB/16384/min_time:5.000             100 us          100 us        69869

BM_FFTW3_DOUBLE/1024/min_time:5.000        1.03 us         1.03 us      6563943
BM_FFTW3_DOUBLE/1331/min_time:5.000        4.20 us         4.20 us      1673972
BM_FFTW3_DOUBLE/1536/min_time:5.000        1.89 us         1.89 us      3666687
BM_FFTW3_DOUBLE/1984/min_time:5.000        12.3 us         12.3 us       553932
BM_FFTW3_DOUBLE/2048/min_time:5.000        2.43 us         2.43 us      2814851
BM_FFTW3_DOUBLE/4096/min_time:5.000        6.82 us         6.82 us      1027944
BM_FFTW3_DOUBLE/8192/min_time:5.000        14.7 us         14.7 us       479778
BM_FFTW3_DOUBLE/11200/min_time:5.000       22.2 us         22.2 us       310204
BM_FFTW3_DOUBLE/11202/min_time:5.000        135 us          135 us        51474
BM_FFTW3_DOUBLE/16384/min_time:5.000       30.1 us         30.1 us       231342

BM_KISSFFT/1024/min_time:5.000             4.25 us         4.25 us      1640712
BM_KISSFFT/1331/min_time:5.000             42.2 us         42.2 us       170796
BM_KISSFFT/1536/min_time:5.000             8.47 us         8.47 us       787339
BM_KISSFFT/1984/min_time:5.000             71.1 us         71.1 us        96450
BM_KISSFFT/2048/min_time:5.000             13.0 us         13.0 us       536075
BM_KISSFFT/4096/min_time:5.000             21.1 us         21.1 us       331449
BM_KISSFFT/8192/min_time:5.000             54.6 us         54.6 us       129238
BM_KISSFFT/11200/min_time:5.000             127 us          127 us        54032
BM_KISSFFT/11202/min_time:5.000           27936 us        27935 us          250
BM_KISSFFT/16384/min_time:5.000            98.5 us         98.5 us        69101
```

## TODO:
- Select FFT backend type (fftw/ne10)
- Add matrix syntax support
- Add custom allocator for `base_array<T>` type
- Add audioread/audiowrite functions (optional libsndfile?)
- Add chain syntax like `fft(x)->abs2()->pow2db()`
- Use `const_span<T>` args