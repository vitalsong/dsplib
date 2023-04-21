#include "dft-tables.h"
#include "datacache.h"

#include <dsplib/math.h>
#include <dsplib/throw.h>

#include <cmath>
#include <vector>
#include <memory>

namespace dsplib {
namespace tables {

//TODO: optional disable caching
static datacache<size_t, fft2tb_ptr> g_dft_cache;
static datacache<size_t, bitrev_ptr> g_bitrev_cache;

//-------------------------------------------------------------------------------------------------
fft2tb_ptr fft2tb::alloc(size_t n) {
    if (n != (1L << nextpow2(n))) {
        DSPLIB_THROW("fft size is not power of 2");
    }

    if (g_dft_cache.cached(n)) {
        return g_dft_cache.get(n);
    }

    auto ptr = fft2tb_ptr(new fft2tb(n));
    g_dft_cache.update(n, ptr);
    return ptr;
}

//-------------------------------------------------------------------------------------------------
void fft2tb::reset(size_t n) {
    g_dft_cache.reset(n);
}

//-------------------------------------------------------------------------------------------------
bool fft2tb::is_cached(size_t n) {
    return g_dft_cache.cached(n);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx fft2tb::unpack() const noexcept {
    arr_cmplx r(_n);

    //real
    for (size_t i = 0; i < _n4; i++) {
        r[i].re = _cos_tb[i];
    }
    r[_n4].re = 0;
    for (size_t i = 0; i < _n4; i++) {
        r[_n4 + 1 + i].re = -_cos_tb[_n4 - i - 1];
    }
    for (size_t i = 0; i < _n2 - 1; i++) {
        r[_n2 + 1 + i].re = r[_n2 - i - 1].re;
    }

    //imag
    const uint32_t ns = (_n - 1);
    for (size_t i = 0; i < _n; i++) {
        r[i].im = r[(i + _n4) & ns].re;
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
fft2tb::fft2tb(uint32_t n) noexcept
  : _n{n}
  , _n2{n / 2}
  , _n4{n / 4}
  , _cos_tb(_n4) {
    assert(n >= 4);
    assert(n == (1L << nextpow2(n)));
    const real_t dt = 1 / real_t(_n);
    for (size_t i = 0; i < _n4; ++i) {
        _cos_tb[i] = std::cos(2 * pi * i * dt);
    }
}

//-------------------------------------------------------------------------------------------------
static inline int _get_bit(int a, int pos) {
    return (a >> pos) & 0x1;
}

//-------------------------------------------------------------------------------------------------
static inline void _set_bit(int& a, int pos, int bit) {
    a &= ~(1 << pos);
    a |= (bit << pos);
}

//-------------------------------------------------------------------------------------------------
static inline int _bitrev(int a, int s) {
    int r = 0;
    for (int i = 0; i < ((s + 1) / 2); ++i) {
        _set_bit(r, (s - i - 1), _get_bit(a, i));
        _set_bit(r, i, _get_bit(a, (s - i - 1)));
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
static bitrev_ptr _gen_bitrev_table(size_t size) {
    auto tb = std::make_shared<std::vector<int32_t>>(size);
    auto data = tb->data();
    const int s = nextpow2(size);
    for (int i = 0; i < size; ++i) {
        int k = _bitrev(i, s);
        if (k > i) {
            data[i] = k;
        } else {
            data[i] = i;
        }
    }

    return tb;
}

//-------------------------------------------------------------------------------------------------
bitrev_ptr bitrev_table(size_t size) {
    if (g_bitrev_cache.cached(size)) {
        return g_bitrev_cache.get(size);
    }

    g_bitrev_cache.update(size, _gen_bitrev_table(size));
    return g_bitrev_cache.get(size);
}

//-------------------------------------------------------------------------------------------------
bool bitrev_cached(size_t n) {
    return g_bitrev_cache.cached(n);
}

//-------------------------------------------------------------------------------------------------
void bitrev_clear(size_t n) {
    g_bitrev_cache.reset(n);
}

}   // namespace tables
}   // namespace dsplib
