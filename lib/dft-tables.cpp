#include "dft-tables.h"
#include "datacache.h"

#include <dsplib/math.h>

#include <math.h>
#include <vector>
#include <memory>

namespace dsplib {
namespace tables {

static datacache<size_t, dft_ptr> g_dft_cache;
static datacache<size_t, bitrev_ptr> g_bitrev_cache;

//-------------------------------------------------------------------------------------------------
static dft_ptr _gen_dft_table(size_t size)
{
    auto tb = std::make_shared<std::vector<cmplx_t>>(size);
    auto data = tb->data();

    double p;
    for (size_t i = 0; i < size; ++i) {
        p = i / double(size);
        data[i].re = ::cos(2 * M_PI * p);
        data[i].im = -::sin(2 * M_PI * p);
    }

    return tb;
}

//-------------------------------------------------------------------------------------------------
const dft_ptr dft_table(size_t size)
{
    if (g_dft_cache.cached(size)) {
        return g_dft_cache.get(size);
    }

    g_dft_cache.update(size, _gen_dft_table(size));
    return g_dft_cache.get(size);
}

//-------------------------------------------------------------------------------------------------
void dft_clear(size_t size)
{
    g_dft_cache.reset(size);
}

//-------------------------------------------------------------------------------------------------
bool dft_cached(size_t size)
{
    return g_dft_cache.cached(size);
}

//-------------------------------------------------------------------------------------------------
static inline int _get_bit(int a, int pos)
{
    return (a >> pos) & 0x1;
}

//-------------------------------------------------------------------------------------------------
static inline void _set_bit(int& a, int pos, int bit)
{
    a &= ~(1 << pos);
    a |= (bit << pos);
}

//-------------------------------------------------------------------------------------------------
static inline int _bitrev(int a, int s)
{
    int r = 0;
    for (int i = 0; i < ((s + 1) / 2); ++i) {
        _set_bit(r, (s - i - 1), _get_bit(a, i));
        _set_bit(r, i, _get_bit(a, (s - i - 1)));
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
static bitrev_ptr _gen_bitrev_table(size_t size)
{
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
const bitrev_ptr bitrev_table(size_t size)
{
    if (g_bitrev_cache.cached(size)) {
        return g_bitrev_cache.get(size);
    }

    g_bitrev_cache.update(size, _gen_bitrev_table(size));
    return g_bitrev_cache.get(size);
}

//-------------------------------------------------------------------------------------------------
bool bitrev_cached(size_t n)
{
    return g_bitrev_cache.cached(n);
}

//-------------------------------------------------------------------------------------------------
void bitrev_clear(size_t n)
{
    g_bitrev_cache.reset(n);
}

}   // namespace tables
}   // namespace dsplib
