#define _CRT_SECURE_NO_WARNINGS

#include <dsplib/utils.h>
#include <dsplib/math.h>
#include <dsplib/fft.h>
#include <dsplib/ifft.h>

#include <algorithm>
#include <cstdio>

namespace dsplib {

namespace {

template<class T>
T _repelem(const T& x, int n) {
    if (n == 0) {
        return T(0);
    }

    if (n == 1) {
        return x;
    }

    T r(x.size() * n);
    for (int i = 0; i < x.size(); i++) {
        int t1 = i * n;
        int t2 = t1 + n;
        std::fill(r.data() + t1, r.data() + t2, x[i]);
    }

    return r;
}

template<typename T>
T _from_bytes(const uint8_t* bytes, endian order);

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

template<>
int16_t _from_bytes(const uint8_t* bytes, endian order) {
    return _from_bytes_16<int16_t>(bytes, order);
}

template<>
uint16_t _from_bytes(const uint8_t* bytes, endian order) {
    return _from_bytes_16<uint16_t>(bytes, order);
}

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

template<>
int32_t _from_bytes(const uint8_t* bytes, endian order) {
    return _from_bytes_32<int32_t>(bytes, order);
}

template<>
uint32_t _from_bytes(const uint8_t* bytes, endian order) {
    return _from_bytes_32<uint32_t>(bytes, order);
}

template<typename T>
arr_real _from_file(const std::string& file, long count, endian order, long offset) {
    FILE* fid = fopen(file.c_str(), "rb");
    DSPLIB_ASSERT(fid != nullptr, "open file error");

    std::array<uint8_t, sizeof(T)> bytes{0};
    fseek(fid, offset, SEEK_CUR);
    std::vector<real_t> res;
    while (!feof(fid) && count) {
        auto rcount = fread(bytes.data(), bytes.size(), 1, fid);
        if (rcount) {
            auto v = _from_bytes<T>(bytes.data(), order);
            res.push_back(v);
            --count;
        }
    }

    fclose(fid);

    return res;
}

template<typename T>
int _finddelay(const base_array<T>& x1, const base_array<T>& x2) {
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

template<typename T>
base_array<T> _circshift(const base_array<T>& x, int k) {
    const int n = x.size();
    if (n == 1 || k == 0) {
        return x;
    }
    base_array<T> y(n);
    for (size_t i = 0; i < n; ++i) {
        const size_t p = (i + k + n) % n;
        y[i] = x[p];
    }
    return y;
}

template<typename T>
base_array<T> _fftshift(const base_array<T>& x) {
    const int n = x.size();
    if (n == 1) {
        return x;
    }
    const int n2 = n / 2;
    dsplib::base_array<T> y(x);
    y.slice(0, n2) = x.slice(n - n2, n);
    y.slice(n2, n) = x.slice(0, n - n2);
    return y;
}

}   // namespace

//-------------------------------------------------------------------------------------------------
arr_real repelem(const arr_real& x, int n) {
    return _repelem<arr_real>(x, n);
}

arr_cmplx repelem(const arr_cmplx& x, int n) {
    return _repelem<arr_cmplx>(x, n);
}

//-------------------------------------------------------------------------------------------------
arr_real flip(const arr_real& x) {
    arr_real r(x);
    std::reverse(r.begin(), r.end());
    return r;
}

arr_cmplx flip(const arr_cmplx& x) {
    arr_cmplx r(x);
    std::reverse(r.begin(), r.end());
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real from_file(const std::string& file, dtype type, endian order, long offset, long count) {
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

//-------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------
int finddelay(const dsplib::arr_real& x1, const dsplib::arr_real& x2) {
    return _finddelay(x1, x2);
}

int finddelay(const dsplib::arr_cmplx& x1, const dsplib::arr_cmplx& x2) {
    return _finddelay(x1, x2);
}

//-------------------------------------------------------------------------------------------------
arr_real linspace(real_t x1, real_t x2, size_t n) {
    DSPLIB_ASSERT(n >= 1, "n must be greater or equal 1");
    if (n == 1) {
        return arr_real{x2};
    }
    if (n == 2) {
        return arr_real{x1, x2};
    }
    const real_t step = (x2 - x1) / (n - 1);
    arr_real out(n);
    for (size_t i = 0; i < n; ++i) {
        out[i] = x1 + (i * step);
    }
    return out;
}

//-------------------------------------------------------------------------------------------------
arr_real circshift(const arr_real& x, int k) {
    return _circshift(x, -k);
}

arr_cmplx circshift(const arr_cmplx& x, int k) {
    return _circshift(x, -k);
}

arr_real fftshift(const arr_real& x) {
    return _fftshift(x);
}

arr_cmplx fftshift(const arr_cmplx& x) {
    return _fftshift(x);
}

}   // namespace dsplib