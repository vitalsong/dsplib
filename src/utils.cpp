#include <dsplib/utils.h>
#include <algorithm>
#include <stdio.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
arr_real range(real_t start, real_t stop, real_t step)
{
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
arr_real range(real_t stop)
{
    return range(0, stop, 1);
}

//-------------------------------------------------------------------------------------------------
template<class T>
T _repelem(const T& x, int n)
{
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
arr_real repelem(const arr_real& x, int n)
{
    return _repelem<arr_real>(x, n);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx repelem(const arr_cmplx& x, int n)
{
    return _repelem<arr_cmplx>(x, n);
}

//-------------------------------------------------------------------------------------------------
arr_real flip(const arr_real& x)
{
    arr_real r(x);
    std::reverse(r.begin(), r.end());
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx flip(const arr_cmplx& x)
{
    arr_cmplx r(x);
    std::reverse(r.begin(), r.end());
    return r;
}

//-------------------------------------------------------------------------------------------------
template<typename T>
T _from_bytes(const uint8_t* bytes, endian order);

//-------------------------------------------------------------------------------------------------
template<typename T>
T _from_bytes_16(const uint8_t* bytes, endian order)
{
    static_assert(sizeof(T) == 2, "The type is the wrong size");

    switch (order) {
    case endian::little:
        return (T(bytes[0]) << 0) | (T(bytes[1]) << 8);

    case endian::big:
        return (T(bytes[0]) << 8) | (T(bytes[1]) << 0);
    }
}

//-------------------------------------------------------------------------------------------------
template<>
int16_t _from_bytes(const uint8_t* bytes, endian order)
{
    return _from_bytes_16<int16_t>(bytes, order);
}

//-------------------------------------------------------------------------------------------------
template<>
uint16_t _from_bytes(const uint8_t* bytes, endian order)
{
    return _from_bytes_16<uint16_t>(bytes, order);
}

//-------------------------------------------------------------------------------------------------
template<typename T>
T _from_bytes_32(const uint8_t* bytes, endian order)
{
    static_assert(sizeof(T) == 4, "The type is the wrong size");

    switch (order) {
    case endian::little:
        return (T(bytes[0]) << 0) | (T(bytes[1]) << 8) | (T(bytes[2]) << 16) | (T(bytes[3]) << 24);

    case endian::big:
        return (T(bytes[0]) << 24) | (T(bytes[1]) << 16) | (T(bytes[2]) << 8) | (T(bytes[3]) << 0);
    }
}

//-------------------------------------------------------------------------------------------------
template<>
int32_t _from_bytes(const uint8_t* bytes, endian order)
{
    return _from_bytes_32<int32_t>(bytes, order);
}

//-------------------------------------------------------------------------------------------------
template<>
uint32_t _from_bytes(const uint8_t* bytes, endian order)
{
    return _from_bytes_32<uint32_t>(bytes, order);
}

//-------------------------------------------------------------------------------------------------
template<typename T>
arr_real _from_file(std::string file, size_t count, endian order, size_t offset)
{
    FILE* fid = fopen(file.c_str(), "rb");
    if (!fid) {
        throw std::runtime_error("open file error");
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
arr_real from_file(std::string file, dtype type, endian order, size_t offset, size_t count)
{
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

}   // namespace dsplib