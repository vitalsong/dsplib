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
T concatenate(const T& a1, const T& a2, const T& a3 = T(), const T& a4 = T(), const T& a5 = T()) {
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
inline arr_real zeros(int n) {
    arr_real r(n);
    std::fill(r.data(), r.data() + n, 0);
    return r;
}

inline arr_real ones(int n) {
    arr_real r(n);
    std::fill(r.data(), r.data() + n, 1);
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
    little,
    big
};

arr_real from_file(std::string file, dtype type = dtype::int16, endian order = endian::little, size_t offset = 0,
                   size_t count = SIZE_MAX);

//----------------------------------------------------------------------------------------------------------
template<typename T>
inline arr_real to_real(const T* x, size_t nx) {
    return dsplib::arr_real(x, nx);
}

//----------------------------------------------------------------------------------------------------------
template<typename T>
inline arr_real to_real(const std::vector<T>& arr) {
    return dsplib::arr_real(arr);
}

//----------------------------------------------------------------------------------------------------------
template<typename T>
inline std::vector<T> from_real(const arr_real& arr) {
    static_assert(std::is_convertible<real_t, T>::value, "Type is not convertible");
    static_assert(is_scalar_ar<T>::value, "Type is not scalar");
    std::vector<T> res(arr.size());
    for (size_t i = 0; i < arr.size(); i++) {
        res[i] = arr[i];
    }
    return res;
}

template<typename T>
inline arr_cmplx to_complex(const T* x, size_t nx) {
    if (nx % 2 != 0) {
        DSPLIB_THROW("Array size is not even");
    }

    const T* p = x;
    arr_cmplx r(nx / 2);
    for (size_t i = 0; i < r.size(); i++) {
        r[i].re = *(p++);
        r[i].im = *(p++);
    }
    return r;
}

template<typename T>
inline arr_cmplx to_complex(const std::vector<T>& arr) {
    static_assert(is_scalar_ar<T>::value, "Type is not scalar");
    return to_complex(arr.data(), arr.size());
}

template<typename T>
inline std::vector<T> from_complex(const arr_cmplx& arr) {
    static_assert(is_scalar_ar<T>::value, "Type is not scalar");
    static_assert(std::is_convertible<real_t, T>::value, "Type is not convertible");
    std::vector<T> res(arr.size() * 2);
    for (size_t i = 0; i < arr.size(); i++) {
        res[2 * i] = arr[i].re;
        res[2 * i + 1] = arr[i].im;
    }
    return res;
}

template<typename T>
inline base_array<T> zeropad(const base_array<T>& x, int n) {
    if (x.size() > n) {
        DSPLIB_THROW("padding size error");
    }

    if (x.size() == n) {
        return x;
    }

    return x | zeros(n - x.size());
}

//delays or advances the signal by the number of samples specified in delay
//TODO: fractional delay support
template<typename T>
inline base_array<T> delayseq(const base_array<T>& data, int delay) {
    if (delay == 0) {
        return data;
    }

    const int N = data.size();
    if (abs(delay) >= N) {
        return zeros(N);
    }

    base_array<T> res = zeros(N);
    if (delay > 0) {
        res.slice(delay, N) = data.slice(0, N - delay);
    } else {
        res.slice(0, N - abs(delay)) = data.slice(abs(delay), N);
    }
    return res;
}

//peak localization
real_t peakloc(const arr_real& x, int idx, bool cyclic = true);
real_t peakloc(const arr_cmplx& x, int idx, bool cyclic = true);

}   // namespace dsplib