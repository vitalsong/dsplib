#include <dft-tables.h>
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

std::vector<int32_t> gen_bitrev(int n) {
    std::vector<int32_t> res(n);
    //TODO: table is symmetry, table(n/2, end) == table(0, n/2)+1
    const int s = nextpow2(n);
    for (int i = 0; i < n; ++i) {
        int k = _bitrev(i, s);
        res[i] = k;
    }
    return res;
}

arr_cmplx gen_coeffs(int n) {
    arr_cmplx res(n);
    for (int i = 0; i < n; ++i) {
        real_t p = i / real_t(n);
        //TODO: only sin operation required (N/8 optimization)
        res[i].re = std::cos(2 * pi * p);
        res[i].im = -std::sin(2 * pi * p);
    }
    return res;
}

}   // namespace

FftParam fft_tables(size_t size) {
    thread_local static FftParam param;

    if (param.coeffs.size() < size) {
        const int nfft = int(1) << nextpow2(size);
        if (nfft != size) {
            DSPLIB_THROW("FFT table len is not power of 2");
        }
        param.coeffs = gen_coeffs(size);
        param.bitrev = gen_bitrev(size);
    }

    const int base_size = param.coeffs.size();
    if (base_size == size) {
        return param;
    }

    //decimate table
    FftParam result = {arr_cmplx(size), std::vector<int32_t>(size)};
    if (base_size % size != 0) {
        DSPLIB_THROW("FFT table len is not multiple");
    }
    const int d = (base_size / size);
    for (int i = 0; i < size; ++i) {
        result.coeffs[i] = param.coeffs[i * d];
        result.bitrev[i] = param.bitrev[i * d];
    }
    return result;
}

}   // namespace dsplib