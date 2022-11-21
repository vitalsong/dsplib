#include <dsplib/utils.h>
#include <dsplib/math.h>
#include <dsplib/fft.h>
#include <dsplib/ifft.h>

#include <algorithm>
#include <cstdio>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
arr_real range(real_t start, real_t stop, real_t step) {
    int n = ::round((stop - start) / step);
    arr_real r(n);
    real_t v = start;
    for (int i = 0; i < n; ++i) {
        r[i] = v;
        v += step;
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real range(real_t stop) {
    return range(0, stop, 1);
}

//-------------------------------------------------------------------------------------------------
template<class T>
T _repelem(const T& x, int n) {
    if (n == 0) {
        return T(0);
    }

    if (n == 1) {
        return x;
    }

    T r(x.size() * n);
    for (size_t i = 0; i < x.size(); i++) {
        int t1 = i * n;
        int t2 = t1 + n;
        std::fill(r.data() + t1, r.data() + t2, x[i]);
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real repelem(const arr_real& x, int n) {
    return _repelem<arr_real>(x, n);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx repelem(const arr_cmplx& x, int n) {
    return _repelem<arr_cmplx>(x, n);
}

//-------------------------------------------------------------------------------------------------
arr_real flip(const arr_real& x) {
    arr_real r(x);
    std::reverse(r.begin(), r.end());
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx flip(const arr_cmplx& x) {
    arr_cmplx r(x);
    std::reverse(r.begin(), r.end());
    return r;
}

//-------------------------------------------------------------------------------------------------
template<typename T>
T _from_bytes(const uint8_t* bytes, endian order);

//-------------------------------------------------------------------------------------------------
template<typename T>
T _from_bytes_16(const uint8_t* bytes, endian order) {
    static_assert(sizeof(T) == 2, "The type is the wrong size");

    switch (order) {
    case endian::little:
        return (T(bytes[0]) << 0) | (T(bytes[1]) << 8);

    case endian::big:
        return (T(bytes[0]) << 8) | (T(bytes[1]) << 0);
    }

    return T();
}

//-------------------------------------------------------------------------------------------------
template<>
int16_t _from_bytes(const uint8_t* bytes, endian order) {
    return _from_bytes_16<int16_t>(bytes, order);
}

//-------------------------------------------------------------------------------------------------
template<>
uint16_t _from_bytes(const uint8_t* bytes, endian order) {
    return _from_bytes_16<uint16_t>(bytes, order);
}

//-------------------------------------------------------------------------------------------------
template<typename T>
T _from_bytes_32(const uint8_t* bytes, endian order) {
    static_assert(sizeof(T) == 4, "The type is the wrong size");

    switch (order) {
    case endian::little:
        return (T(bytes[0]) << 0) | (T(bytes[1]) << 8) | (T(bytes[2]) << 16) | (T(bytes[3]) << 24);

    case endian::big:
        return (T(bytes[0]) << 24) | (T(bytes[1]) << 16) | (T(bytes[2]) << 8) | (T(bytes[3]) << 0);
    }

    return T();
}

//-------------------------------------------------------------------------------------------------
template<>
int32_t _from_bytes(const uint8_t* bytes, endian order) {
    return _from_bytes_32<int32_t>(bytes, order);
}

//-------------------------------------------------------------------------------------------------
template<>
uint32_t _from_bytes(const uint8_t* bytes, endian order) {
    return _from_bytes_32<uint32_t>(bytes, order);
}

//-------------------------------------------------------------------------------------------------
template<typename T>
arr_real _from_file(std::string file, size_t count, endian order, size_t offset) {
    FILE* fid = fopen(file.c_str(), "rb");
    if (!fid) {
        DSPLIB_THROW("open file error");
    }

    uint8_t bytes[sizeof(T)];
    fseek(fid, offset, SEEK_CUR);
    arr_real res;
    while (!feof(fid) && count) {
        auto rcount = fread(bytes, sizeof(T), 1, fid);
        if (rcount) {
            auto v = _from_bytes<T>(bytes, order);
            res.push_back(v);
            --count;
        }
    }

    fclose(fid);

    return res;
}

//-------------------------------------------------------------------------------------------------
arr_real from_file(std::string file, dtype type, endian order, size_t offset, size_t count) {
    switch (type) {
    case dtype::int16:
        return _from_file<int16_t>(file, count, order, offset);
    case dtype::uint16:
        return _from_file<uint16_t>(file, count, order, offset);
    case dtype::int32:
        return _from_file<int32_t>(file, count, order, offset);
    case dtype::uint32:
        return _from_file<uint32_t>(file, count, order, offset);
    default:
        return arr_real{};
    }
}

real_t peakloc(const arr_real& x, int idx, bool cyclic) {
    const int n = x.size();
    if (!cyclic && (idx == 0 || idx == n - 1)) {
        return idx;
    }

    int ml = (idx - 1 + n) % n;
    int mk = idx;
    int mr = (idx + 1) % n;

    real_t a = ((x[mr] - x[mk]) + (x[ml] - x[mk])) / 2;
    real_t b = x[mk] - a - x[ml];
    real_t q = (-b);
    return idx + q / (2 * a) - 1;
}

real_t peakloc(const arr_cmplx& x, int idx, bool cyclic) {
    const int n = x.size();
    if (!cyclic && (idx == 0 || idx == n - 1)) {
        return idx;
    }

    int ml = (idx - 1 + n) % n;
    int mk = idx;
    int mr = (idx + 1) % n;

    auto d = (x[mr] - x[ml]) / (2 * x[mk] - x[ml] - x[mr]);
    return mk - real(d);
}

template<typename T>
static int _finddelay(const base_array<T>& x1, const base_array<T>& x2) {
    const int max_lag = max(x1.size(), x2.size());
    const int nfft = 1L << nextpow2(max_lag);
    const auto s1 = zeropad(x1, nfft);
    const auto s2 = zeropad(x2, nfft);
    const auto S1 = fft(s1);
    const auto S2 = fft(s2);
    const auto s = ifft(S1 * conj(S2));
    int delay = argmax(s);
    if (delay > nfft / 2) {
        delay = -(nfft - delay);
    }
    return -delay;
}

int finddelay(const dsplib::arr_real& x1, const dsplib::arr_real& x2) {
    return _finddelay(x1, x2);
}

int finddelay(const dsplib::arr_cmplx& x1, const dsplib::arr_cmplx& x2) {
    return _finddelay(x1, x2);
}

}   // namespace dsplib