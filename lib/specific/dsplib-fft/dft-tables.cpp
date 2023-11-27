#include "dft-tables.h"

#include <dsplib/throw.h>
#include <dsplib/math.h>

#include <cmath>
#include <vector>
#include <memory>

namespace dsplib {

namespace {

inline int _get_bit(int a, int pos) {
    return (a >> pos) & 0x1;
}

inline void _set_bit(int& a, int pos, int bit) {
    a &= ~(1 << pos);
    a |= (bit << pos);
}

inline int _bitrev(int a, int s) {
    int r = 0;
    for (int i = 0; i < ((s + 1) / 2); ++i) {
        _set_bit(r, (s - i - 1), _get_bit(a, i));
        _set_bit(r, i, _get_bit(a, (s - i - 1)));
    }
    return r;
}

//generate a half of bitrev table
//table is symmetry, table(n/2, n) == table(0, n/2)+1
std::vector<int32_t> _gen_bitrev(const int n) {
    std::vector<int32_t> res(n / 2);
    const int s = nextpow2(n);
    for (int i = 0; i < n / 2; ++i) {
        res[i] = _bitrev(i, s);
    }
    return res;
}

std::vector<real_t> _gen_coeffs(const int n) {
    std::vector<real_t> res(n);
    //TODO: N/4 optimization
    for (int i = 0; i < n; ++i) {
        res[i] = std::cos(2 * pi * i / n);
    }
    return res;
}

}   // namespace

FftParam fft_tables(size_t size) {
    thread_local static FftParam base_prm;

    if (base_prm.size < size) {
        const int nfft = int(1) << nextpow2(size);
        if (nfft != size) {
            DSPLIB_THROW("FFT table len is not power of 2");
        }
        base_prm.coeffs = _gen_coeffs(size);
        base_prm.bitrev = _gen_bitrev(size);
        base_prm.size = size;
    }

    if (base_prm.size == size) {
        return base_prm;
    }

    if (base_prm.size % size != 0) {
        DSPLIB_THROW("FFT table len is not multiple");
    }

    //decimate table
    FftParam result;
    result.coeffs.resize(size);
    result.bitrev.resize(size / 2);
    result.size = size;

    const int d = (base_prm.size / size);
    for (int i = 0, k = 0; i < size; ++i, k += d) {
        result.coeffs[i] = base_prm.coeffs[k];
    }
    for (int i = 0, k = 0; i < size / 2; ++i, k += d) {
        result.bitrev[i] = base_prm.bitrev[k];
    }
    return result;
}

}   // namespace dsplib