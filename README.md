# DSPLIB
![testing](https://github.com/vitalsong/dsplib/actions/workflows/android-ndk.yml/badge.svg)
![testing](https://github.com/vitalsong/dsplib/actions/workflows/linux.yml/badge.svg)
![testing](https://github.com/vitalsong/dsplib/actions/workflows/windows.yml/badge.svg)

C++ DSP library for MATLAB similar programming.

Note: Only 1-D arrays with element-wise operations are currently supported.

| dsplib      | matlab      | numpy       |
| ----------- | ----------- | ----------- |
| x * x       | x .* x      | x * x |
| zeros(20)   | zeros(20, 1) | zeros(20) |
| x.slice(0,10) = 1 | x(1:10) = 1 | x[0:10] = 1 |
| x.slice(2,end) = 1 | x(3:end) = 1 | x[2:] = 1 |
| x.slice(2, 4) = {1, 2} | x(3:4) = [1, 2] | x[2:4] = [1, 2] |
| x.slice(0, -1) | x(1:end-1) | x[0:-1] |

Matrix support is planned for the future.

## Usage

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
The FFT/IFFT calculation table is cached on first run. To eliminate this behavior, you can use the fft_plan object.
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

### Spectrum Analyze (16-bit scale):
```cpp
const int n = 1024;
arr_real x = randn(n);
x *= window::hann(n);
arr_cmplx y = fft(x) / (n / 2);
y = y.slice(0, n/2);
auto z = mag2db(abs(y) / 0x7FFF);   //20*log10(..)
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
