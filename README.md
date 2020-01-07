# DSPLIB

C++ DSP library for MATLAB similar programming.

## Usage

Basic operations:
```cpp
using namespace dsplib;
int n = 100;
arr_real x1 = arr_real(n);
arr_cmplx x2 = arr_cmplx(n);
x1[0] = 10;
x2[0] = {10, 10};
x2[1].xi = 20;
x2[1].xq = 20;
arr_real y1 = x1 * x1;
arr_real y2 = x1 * 1000;
arr_cmplx y3 = x1 * x2;
arr_cmplx y4 = x2 * 1000;
arr_cmplx y5 = x2.slice(0, 10);
arr_cmplx y6 = x1 * cmplx_t{0, 1};
```

Fast Fourier Transform:
```cpp
dsplib::arr_real x = dsplib::randn(512);
dsplib::arr_cmplx y = dsplib::fft(x);
```

Inverse Fast Fourier Transform:
```cpp
dsplib::arr_cmplx x = dsplib::arr_cmplx::zeros(512);
x[10] = 1;
dsplib::arr_cmplx y = dsplib::ifft(x);
```

FIR filter:
```cpp 
const double IR[4] = {1, 0, 0, 0};
dsplib::arr_real h = dsplib::arr_real(IR, 4);
auto flt = dsplib::fir(h);
dsplib::arr_real x = dsplib::randn(10000);
dsplib::arr_real y = flt.filter(x);
```

Hilbert filter:
```cpp
auto flt = dsplib::hilbert();
dsplib::arr_real x = dsplib::randn(10000);
dsplib::arr_cmplx y1 = flt.filter(x); //sequence
//or
dsplib::arr_cmplx y2 = hilbert::process(x);
```

Add White Gaussian Noise:
```cpp
dsplib::arr_real x = dsplib::randn(10000);
dsplib::arr_real y = dsplib::awgn(x, 10);   //10dB
```

Cross-correlation:
```cpp
dsplib::arr_real x1 = dsplib::randn(500);
dsplib::arr_real x2 = dsplib::awgn(x1, 10);
dsplib::arr_real y = dsplib::xcorr(x1, x2);
```

Simple Spectrum Analyze (16-bit scale):
```cpp
int nfft = 1024;
dsplib::arr_real x = dsplib::randn(nfft) * 1000;
dsplib::arr_real w = dsplib::window::hann(nfft);
x *= w;
dsplib::arr_cmplx y = dsplib::fft(x) / (nfft / 2);
dsplib::arr_real z = dsplib::abs(y);
z = dsplib::log10(z / 0x7FFF) * 20;
```
