#include <dsplib.h>

namespace dsplib {

namespace {

void _dft_n2(cmplx_t* x) noexcept {
    cmplx_t y0, y1;

    y0.re = x[0].re + x[1].re;
    y0.im = x[0].im + x[1].im;

    y1.re = (x[0].re - x[1].re);
    y1.im = (x[0].im - x[1].im);

    x[0] = y0;
    x[1] = y1;
}

void _dft_n3(cmplx_t* x) noexcept {
    cmplx_t y0, y1, y2;
    constexpr real_t c = -0.5;
    constexpr real_t d = 0.866025403784439;

    y0.re = x[0].re + x[1].re + x[2].re;
    y0.im = x[0].im + x[1].im + x[2].im;

    const real_t re1_c = x[1].re * c;
    const real_t im1_d = x[1].im * d;
    const real_t re2_c = x[2].re * c;
    const real_t im2_d = x[2].im * d;
    y1.re = x[0].re + (re1_c + im1_d) + (re2_c - im2_d);
    y2.re = x[0].re + (re1_c - im1_d) + (re2_c + im2_d);

    const real_t re1_d = x[1].re * d;
    const real_t im1_c = x[1].im * c;
    const real_t re2_d = x[2].re * d;
    const real_t im2_c = x[2].im * c;
    y1.im = x[0].im + (-re1_d + im1_c) + (re2_d + im2_c);
    y2.im = x[0].im + (re1_d + im1_c) + (-re2_d + im2_c);

    x[0] = y0;
    x[1] = y1;
    x[2] = y2;
}

cmplx_t _dot(const cmplx_t* x, const cmplx_t* w, int n) {
    cmplx_t r{0, 0};
    for (int i = 0; i < n; ++i) {
        r += x[i] * w[i];
    }
    return r;
}

void _dft_n5(cmplx_t* x) noexcept {
    //TODO: optimization
    using namespace std::complex_literals;
    static const cmplx_t w1[5] = {1.00000000000000 + 0.00000000000000i, 0.309016994374947 - 0.951056516295154i,
                                  -0.809016994374947 - 0.587785252292473i, -0.809016994374948 + 0.587785252292473i,
                                  0.309016994374947 + 0.951056516295154i};

    static const cmplx_t w2[5] = {1.00000000000000 + 0.00000000000000i, -0.809016994374947 - 0.587785252292473i,
                                  0.309016994374947 + 0.951056516295154i, 0.309016994374948 - 0.951056516295154i,
                                  -0.809016994374948 + 0.587785252292473i};

    static const cmplx_t w3[5] = {1.00000000000000 + 0.00000000000000i, -0.809016994374948 + 0.587785252292473i,
                                  0.309016994374948 - 0.951056516295154i, 0.309016994374947 + 0.951056516295154i,
                                  -0.809016994374947 - 0.587785252292474i};

    static const cmplx_t w4[5] = {1.00000000000000 + 0.00000000000000i, 0.309016994374947 + 0.951056516295154i,
                                  -0.809016994374948 + 0.587785252292473i, -0.809016994374947 - 0.587785252292474i,
                                  0.309016994374948 - 0.951056516295153i};

    cmplx_t y0, y1, y2, y3, y4;

    y0.re = x[0].re + x[1].re + x[2].re + x[3].re + x[4].re;
    y0.im = x[0].im + x[1].im + x[2].im + x[3].im + x[4].im;

    y1 = _dot(x, w1, 5);
    y2 = _dot(x, w2, 5);
    y3 = _dot(x, w3, 5);
    y4 = _dot(x, w4, 5);

    x[0] = y0;
    x[1] = y1;
    x[2] = y2;
    x[3] = y3;
    x[4] = y4;
}

void _dft_slow(cmplx_t* x, int n) noexcept {
    //TODO: use cached table
    const arr_cmplx w = expj(-2 * pi * arange(n) / n);
    arr_cmplx y(n);

    for (int i = 0; i < n; ++i) {
        y[0] += x[i];
    }

    for (int k = 1; k < n; ++k) {
        int iw = 0;
        for (int i = 0; i < n; ++i) {
            y[k] += x[i] * w[iw];
            iw = (iw + k);
            iw = (iw < n) ? iw : (iw - n);
        }
    }

    std::memcpy(x, y.data(), n * sizeof(cmplx_t));
}

void _transpose(cmplx_t* x, cmplx_t* t, int nrow, int ncol) noexcept {
    std::memcpy(t, x, nrow * ncol * sizeof(cmplx_t));
    for (int i = 0; i < nrow; ++i) {
        for (int j = 0; j < ncol; ++j) {
            const int src = (i * ncol) + j;
            const int dst = (j * nrow) + i;
            x[dst] = t[src];
        }
    }
}

}   // namespace

//-----------------------------------------------------------------------------------------------------------------------------
FactorFFTPlan::FactorFFTPlan(int n)
  : _n{n}
  , _px(n) {
    _factors = factor(n);
    DSPLIB_ASSERT(_factors.size() >= 2, "fft size must not be a prime number");
    _twiddle = expj(-2 * pi * arange(n) / n);   // TODO: use only half
    _tw.reserve(n);
}

[[nodiscard]] arr_cmplx FactorFFTPlan::solve(const arr_cmplx& x) const {
    DSPLIB_ASSERT(x.size() == _n, "input vector size is not equal fft size");
    arr_cmplx r(x);
    _fft(r.data(), _n, 0);
    return r;
}

[[nodiscard]] int FactorFFTPlan::size() const noexcept {
    return _n;
}

void FactorFFTPlan::_dft(cmplx_t* x, int n) const noexcept {
    //TODO: noexcept?
    if (n == 2) {
        _dft_n2(x);
    } else if (n == 3) {
        _dft_n3(x);
    } else if (n == 5) {
        _dft_n5(x);
    } else if (n < 31) {
        _dft_slow(x, n);
    } else {
        //TODO: call czt without copy, instance czt plan for last factor
        const auto y = fft(arr_cmplx(x, n));
        std::memcpy(x, y.data(), n * sizeof(cmplx_t));
    }
}

void FactorFFTPlan::_update_table(int P, int Q) const noexcept {
    //TODO: cache table here
    //TODO: use only base table with steps
    //TODO: optimization for simple P/Q combinations (2,2 or 3,3)

    const int n = P * Q;

    if (_tw.size() == n) {
        return;
    }

    const int decim = _n / n;
    _tw.resize(n);
    for (int p = 0; p < P; ++p) {
        for (int q = 0; q < Q; ++q) {
            const int idx = q * p * decim;
            _tw[q * P + p] = _twiddle[idx];
        }
    }
}

void FactorFFTPlan::_fft(cmplx_t* x, int n, int depth) const noexcept {
    if (depth == _factors.size() - 1) {
        _dft(x, n);
        return;
    }

    const int Q = _factors[depth];
    const int P = n / Q;

    //TODO: ignore this transpose and previous?
    _transpose(x, _px.data(), P, Q);

    //inner fft (recursive)
    for (int k = 0; k < Q; ++k) {
        auto* px = x + (k * P);
        _fft(px, P, depth + 1);
    }

    //generate twiddle table (cached for current P * Q)
    _update_table(P, Q);

    //multiple by twiddle (ignore first row, because it is always 1.0)
    //TODO: ignore first column
    for (int k = P; k < (P * Q); ++k) {
        x[k] *= _tw[k];
    }

    //outer fft
    _transpose(x, _px.data(), Q, P);
    for (int k = 0; k < P; ++k) {
        auto* px = x + (k * Q);
        _dft(px, Q);
    }

    //transpose
    _transpose(x, _px.data(), P, Q);
}

}   // namespace dsplib