#include <dsplib/fir.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>
#include <dsplib/fft.h>
#include <dsplib/ifft.h>
#include <dsplib/window.h>
#include <cassert>

namespace dsplib {

namespace {

template<class T>
void _conv(T* restrict x, const T* restrict h, int nh, int nx) {
    const int nr = nx - nh + 1;
    DSPLIB_ASSUME(nr > 0);
    for (int i = 0; i < nr; ++i) {
        T r = 0;
        for (int k = 0; k < nh; ++k) {
            r += x[i + k] * conj(h[nh - k - 1]);
        }
        x[i] = r;
    }
}

arr_real _lowpass_fir(int n, real_t wn, const arr_real& win) {
    if (win.size() != (n + 1)) {
        DSPLIB_THROW("Window must be n+1 elements");
    }

    const bool is_odd = (n % 2 == 1);
    const int M = win.size();
    const int L = M / 2;
    const real_t fc = wn / 2;
    const arr_real w = win.slice(0, L);

    auto tt = arange(L) - real_t(n) / 2;
    auto h = zeros(M);
    h.slice(0, L) = sin(2 * pi * fc * tt) / tt * w;
    if (!is_odd) {
        h[L] = 2 * pi * fc;
        h.slice(L + 1, M) = flip(h.slice(0, L));
    } else {
        h.slice(L, M) = flip(h.slice(0, L));
    }

    h /= sum(h);
    return h;
}

arr_real _highpass_fir(int n, real_t wn, const arr_real& win) {
    wn = 1 - wn;
    int t1 = 0;
    if (n % 2 == 1) {
        n += 1;
        t1 = 1;
    }

    auto h = _lowpass_fir(n, wn, win);
    auto hh = arr_real(h.slice(t1, n, 2));
    h.slice(t1, n, 2) = -hh;
    return h;
}

arr_real _bandpass_fir(int n, real_t wn1, real_t wn2, const arr_real& win) {
    wn1 = wn1 / 2;
    wn2 = wn2 / 2;
    auto wp = (wn2 - wn1) / 2;
    auto wc = wn1 + wp;
    auto h = _lowpass_fir(n, 2 * wp, win);
    auto t = arange(h.size()) - real_t(n) / 2;
    h = 2 * h * cos(2 * pi * wc * t);
    return h;
}

arr_real _bandstop_fir(int n, real_t wn1, real_t wn2, const arr_real& win) {
    if (n % 2 == 1) {
        n += 1;
    }

    auto h = (-1) * _bandpass_fir(n, wn1, wn2, win);
    h[n / 2] = h[n / 2] + 1;
    return h;
}

bool _equal(const real_t& x1, const real_t& x2) {
    return (abs(x1 - x2) < 2 * eps());
}

bool _is_symmetric(const dsplib::arr_real& h) {
    const int n = h.size();
    for (int i = 0; i < n / 2; i++) {
        if (!_equal(h[i], h[n - i - 1])) {
            return false;
        }
    }
    return true;
}

bool _is_antisymmetric(const dsplib::arr_real& h) {
    const int n = h.size();
    for (int i = 0; i < n / 2; i++) {
        if (!_equal(h[i], -h[n - i - 1])) {
            return false;
        }
    }
    return true;
}

}   // namespace

//-------------------------------------------------------------------------------------------------
template<>
void FirFilter<real_t>::conv(mut_span_t<real_t> x, span_t<real_t> h) {
    _conv(x.data(), h.data(), h.size(), x.size());
}
template<>
void FirFilter<cmplx_t>::conv(mut_span_t<cmplx_t> x, span_t<cmplx_t> h) {
    _conv(x.data(), h.data(), h.size(), x.size());
}

//-------------------------------------------------------------------------------------------------
FftFilter::FftFilter(const arr_cmplx& h)
  : _m{h.size()} {
    const int fft_len = 1L << nextpow2(2 * h.size());
    _n = fft_len - h.size() + 1;
    assert(_n > _m);
    _olap = complex(zeros(_m - 1));
    _h = fft(conj(h), fft_len);
    _x = complex(zeros(fft_len));
}

FftFilter::FftFilter(const arr_real& h)
  : FftFilter(complex(h)) {
}

arr_cmplx FftFilter::process(const arr_cmplx& x) {
    const int nr = (x.size() + _nx) / _n * _n;
    arr_cmplx r(nr);
    cmplx_t* pr = r.data();
    for (const auto& val : x) {
        _x[_nx] = val;
        _nx += 1;
        if (_nx == _n) {
            const auto ry = ifft(fft(_x) * _h);

            for (int i = 0; i < _n; i++) {
                pr[i] = ry[i];
            }

            for (int i = 0; i < (_m - 1); i++) {
                pr[i] += _olap[i];
                _olap[i] = ry[i + _n];
            }

            pr += _n;
            _nx = 0;
        }
    }
    return r;
}

arr_real FftFilter::process(const arr_real& x) {
    //TODO: real optimization
    return real(process(complex(x)));
}

//----------------------------------------------------------------------------------------------
arr_real fir1(int n, real_t wn, FilterType ftype, const arr_real& win) {
    assert(n > 0);
    assert((wn > 0) && (wn < 1));

    if (ftype == FilterType::Low) {
        return _lowpass_fir(n, wn, win);
    }

    if (ftype == FilterType::High) {
        return _highpass_fir(n, wn, win);
    }

    DSPLIB_THROW("Not supported for current filter type");
}

arr_real fir1(int n, real_t wn1, real_t wn2, FilterType ftype, const arr_real& win) {
    assert(n > 0);
    assert(wn1 < wn2);
    assert((wn1 > 0) && (wn1 < 1));

    if (ftype == FilterType::Bandpass) {
        return _bandpass_fir(n, wn1, wn2, win);
    }

    if (ftype == FilterType::Bandstop) {
        return _bandstop_fir(n, wn1, wn2, win);
    }

    DSPLIB_THROW("Not supported for current filter type");
}

arr_real fir1(int n, real_t wn, FilterType ftype) {
    DSPLIB_ASSERT((ftype == FilterType::High) || (ftype == FilterType::Low), "Not supported for current filter type");
    const int nn = ((n % 2 == 1) && (ftype == FilterType::High)) ? (n + 2) : (n + 1);
    return fir1(n, wn, ftype, window::hamming(nn));
}

arr_real fir1(int n, real_t wn1, real_t wn2, FilterType ftype) {
    DSPLIB_ASSERT((ftype == FilterType::Bandstop) || (ftype == FilterType::Bandpass),
                  "Not supported for current filter type");
    const int nn = ((n % 2 == 1) && (ftype == FilterType::Bandstop)) ? (n + 2) : (n + 1);
    return fir1(n, wn1, wn2, ftype, window::hamming(nn));
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
