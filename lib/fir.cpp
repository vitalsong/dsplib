#include <dsplib/fir.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>
#include <dsplib/fft.h>
#include <dsplib/ifft.h>
#include <dsplib/window.h>
#include <cassert>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
fir::fir() {
    _h = arr_real(1);
    _h[0] = 1;
}

//-------------------------------------------------------------------------------------------------
fir_cmplx::fir_cmplx() {
    _h = arr_cmplx(1);
    _h[0] = 1;
}

//-------------------------------------------------------------------------------------------------
fir::fir(const arr_real& h) {
    _h = h;
    _d = zeros(h.size() - 1);
}

//-------------------------------------------------------------------------------------------------
fir_cmplx::fir_cmplx(const arr_cmplx& h) {
    _h = h;
    _d = arr_cmplx(zeros(h.size() - 1));
}

//-------------------------------------------------------------------------------------------------
arr_real fir::process(const arr_real& s) {
    auto x = _d | s;

    //convolution
    arr_real r = fir::conv(x, _h);

    //keep filter delay
    int nd = _h.size() - 1;
    int nx = x.size();
    _d = x.slice((nx - nd), nx);

    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fir_cmplx::process(const arr_cmplx& s) {
    auto x = _d | s;

    //convolution
    arr_cmplx r = fir_cmplx::conv(x, _h);

    //keep filter delay
    int nd = _h.size() - 1;
    int nx = x.size();
    _d = x.slice((nx - nd), nx);

    return r;
}

//-------------------------------------------------------------------------------------------------
template<class T>
static void _conv(const T* restrict x, const T* restrict h, T* restrict r, int nh, int nx) {
    const int nr = nx - nh + 1;
    assert(nr > 0);
    for (int i = 0; i < nr; ++i) {
        r[i] = 0;
        for (int k = 0; k < nh; ++k) {
            r[i] += x[i + k] * conj(h[nh - k - 1]);
        }
    }
}

//-------------------------------------------------------------------------------------------------
arr_real fir::conv(const arr_real& x, const arr_real& h) {
    arr_real r(x.size() - h.size() + 1);
    _conv(x.data(), h.data(), r.data(), h.size(), x.size());
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fir_cmplx::conv(const arr_cmplx& x, const arr_cmplx& h) {
    arr_cmplx r(x.size() - h.size() + 1);
    _conv(x.data(), h.data(), r.data(), h.size(), x.size());
    return r;
}

//-------------------------------------------------------------------------------------------------
fir_fft::fir_fft(const arr_cmplx& h) {
    int fft_len = 1L << nextpow2(2 * h.size());
    _n = fft_len - h.size();
    _m = h.size();
    _olap = zeros(_m);
    auto dh = conj(h) | zeros(fft_len - h.size());
    _h = fft(dh);
    _x = zeros(fft_len);
}

//-------------------------------------------------------------------------------------------------
fir_fft::fir_fft(const arr_real& h)
  : fir_fft(arr_cmplx(h)) {
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fir_fft::process(const arr_cmplx& x) {
    if (_n == 0) {
        return x;
    }

    const int nr = (x.size() + _nx) / _n * _n;
    arr_cmplx r(nr);
    cmplx_t* pr = r.data();
    for (const auto& val : x) {
        _x[_nx] = val;
        _nx += 1;
        if (_nx == _n) {
            const auto yy = fft(_x) * _h;
            auto ry = ifft(yy);
            for (size_t i = 0; i < _m; i++) {
                ry[i] += _olap[i];
            }
            for (size_t i = 0; i < _m; i++) {
                _olap[i] = ry[i + _n];
            }
            for (size_t i = 0; i < _n; i++) {
                pr[i] = ry[i];
            }
            pr += _n;
            _nx = 0;
        }
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real fir_fft::process(const arr_real& x) {
    return real(process(arr_cmplx(x)));
}

//----------------------------------------------------------------------------------------------
static arr_real _lowpass_fir(int n, real_t wn) {
    const bool is_odd = (n % 2 == 1);
    const int M = n + 1;
    const int L = M / 2;
    const real_t fc = wn / 2;
    const arr_real win = window::hamming(M).slice(0, L);

    auto tt = range(L) - real_t(n) / 2;
    auto h = zeros(M);
    h.slice(0, L) = sin(2 * pi * fc * tt) / tt * win;
    if (!is_odd) {
        h(L) = 2 * pi * fc;
        h.slice(L + 1, M) = flip(h.slice(0, L));
    } else {
        h.slice(L, M) = flip(h.slice(0, L));
    }

    h /= sum(h);
    return h;
}

//----------------------------------------------------------------------------------------------
static arr_real _highpass_fir(int n, real_t wn) {
    wn = 1 - wn;
    int t1 = 0;
    if (n % 2 == 1) {
        n += 1;
        t1 = 1;
    }
    auto h = _lowpass_fir(n, wn);
    auto hh = arr_real(h.slice(t1, n, 2));
    h.slice(t1, n, 2) = -hh;
    return h;
}

//----------------------------------------------------------------------------------------------
static arr_real _bandpass_fir(int n, real_t wn1, real_t wn2) {
    wn1 = wn1 / 2;
    wn2 = wn2 / 2;
    auto wp = (wn2 - wn1) / 2;
    auto wc = wn1 + wp;
    auto h = _lowpass_fir(n, 2 * wp);
    auto t = range(h.size()) - real_t(n) / 2;
    h = 2 * h * cos(2 * pi * wc * t);
    return h;
}

//----------------------------------------------------------------------------------------------
static arr_real _bandstop_fir(int n, real_t wn1, real_t wn2) {
    if (n % 2 == 1) {
        n += 1;
    }
    auto h = _bandpass_fir(n, wn1, wn2);
    h = -h;
    h(n / 2) = h(n / 2) + 1;
    return h;
}

//----------------------------------------------------------------------------------------------
arr_real fir1(int n, real_t wn, FilterType ftype) {
    assert(n > 0);
    assert((wn > 0) && (wn < 1));

    if (ftype == FilterType::Low) {
        return _lowpass_fir(n, wn);
    }

    if (ftype == FilterType::High) {
        return _highpass_fir(n, wn);
    }

    DSPLIB_THROW("Not supported for current filter type");
    return {};
}

//----------------------------------------------------------------------------------------------
arr_real fir1(int n, real_t wn1, real_t wn2, FilterType ftype) {
    assert(n > 0);
    assert(wn1 < wn2);
    assert((wn1 > 0) && (wn1 < 1));

    if (ftype == FilterType::Bandpass) {
        return _bandpass_fir(n, wn1, wn2);
    }

    if (ftype == FilterType::Bandstop) {
        return _bandstop_fir(n, wn1, wn2);
    }

    DSPLIB_THROW("Not supported for current filter type");
    return {};
}

//----------------------------------------------------------------------------------------------
static bool _equal(const real_t& x1, const real_t& x2) {
    return (abs(x1 - x2) < 2 * eps());
}

//----------------------------------------------------------------------------------------------
static bool _is_symmetric(const dsplib::arr_real& h) {
    const int n = h.size();
    for (int i = 0; i < n / 2; i++) {
        if (!_equal(h[i], h[n - i - 1])) {
            return false;
        }
    }
    return true;
}

//----------------------------------------------------------------------------------------------
static bool _is_antisymmetric(const dsplib::arr_real& h) {
    const int n = h.size();
    for (int i = 0; i < n / 2; i++) {
        if (!_equal(h[i], -h[n - i - 1])) {
            return false;
        }
    }
    return true;
}

//----------------------------------------------------------------------------------------------
FirType firtype(const dsplib::arr_real& h) {
    const int n = h.size();

    if (n == 1) {
        return FirType::EvenSymm;
    }

    const bool is_sym = _is_symmetric(h);
    const bool is_asym = _is_antisymmetric(h);
    const bool is_even = (n % 2 == 1);

    if (is_even && is_sym) {
        return FirType::EvenSymm;
    }

    if (!is_even && is_sym) {
        return FirType::OddSym;
    }

    if (is_even && is_asym && _equal(h[n / 2], 0)) {
        return FirType::EvenAntiSym;
    }

    if (!is_even && is_asym) {
        return FirType::OddAntiSym;
    }

    return FirType::NonlinearPhase;
}

}   // namespace dsplib
