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
using namespace dsplib;
arr_real x = randn(512);
arr_cmplx y = fft(x);
```

### Inverse Fast Fourier Transform:
```cpp
arr_cmplx x = 1i * zeros(512);
x[10] = 1;
arr_cmplx y = ifft(x);
```

### FIR filter:
```cpp
const double IR[4] = {1, 0, 0, 0};
arr_real h = arr_real(IR, 4);
auto flt = fir(h);
arr_real x = randn(10000);
arr_real y = flt(x);
```

### Hilbert filter:
```cpp
auto flt = hilbert_filter();
arr_real x = randn(10000);
arr_cmplx y1 = flt(x);
```

### Add White Gaussian Noise:
```cpp
arr_real x = randn(10000);
arr_real y = awgn(x, 10);   //10dB
```

### Cross-correlation:
```cpp
arr_real x1 = randn(500);
arr_real x2 = awgn(x1, 10);
arr_real y = xcorr(x1, x2);
```

Simple Spectrum Analyze (16-bit scale):
```cpp
int nfft = 1024;
arr_real x = randn(nfft) * 1000;
arr_real w = window::hann(nfft);
x *= w;
arr_cmplx y = fft(x) / (nfft / 2);
arr_real z = abs(y);
z = log10(z / 0x7FFF) * 20;
```

## Allocation

On some platforms (for example, Android) there may be performance issues due to slow memory allocation for the vector. In this case, try enabling a custom allocator via the DSPLIB_POOL_ALLOCATOR option and estimate its peak consumption using the dsplib::pool_state function.
