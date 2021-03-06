#pragma once

#include <dsplib/array.h>
#include <array>

namespace dsplib {

//sequence of numbers (not include stop)
arr_real range(real_t start, real_t stop, real_t step = 1);
arr_real range(real_t stop);

//join a sequence of arrays
//TODO: add impl for slices
template<class T>
T concatenate(const T& a1, const T& a2, const T& a3 = T(), const T& a4 = T(), const T& a5 = T())
{
    std::array<const T*, 5> arrays{&a1, &a2, &a3, &a4, &a5};

    size_t nr = 0;
    for (auto array : arrays) {
        nr += array->size();
    }

    T r(nr);
    auto* pr = r.data();
    for (auto array : arrays) {
        const auto* pa = array->data();
        for (int i = 0; i < array->size(); ++i) {
            *pr = pa[i];
            pr++;
        }
    }

    return r;
}

//create array of all zeros
inline arr_real zeros(int n)
{
    arr_real r(n);
    std::fill(r.data(), r.data() + n, 0);
    return r;
}

//repeat copies of array elements
arr_real repelem(const arr_real& x, int n);
arr_cmplx repelem(const arr_cmplx& x, int n);

//flip order of elements
arr_real flip(const arr_real& x);
arr_cmplx flip(const arr_cmplx& x);

enum class dtype
{
    int16,
    uint16,
    int32,
    uint32
};

enum class endian
{
    little = LITTLE_ENDIAN,
    big = BIG_ENDIAN
};

arr_real from_file(std::string file, dtype type = dtype::int16, endian order = endian::little, size_t offset = 0,
                   size_t count = SIZE_MAX);

}   // namespace dsplib