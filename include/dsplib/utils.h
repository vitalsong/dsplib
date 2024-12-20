#pragma once

#include <dsplib/array.h>

#include <array>
#include <cstdint>

namespace dsplib {

//sequence of numbers (not include "stop")
template<typename T1, typename T2, typename T3>
using enable_if_some_float_t = std::enable_if<std::is_floating_point<T1>::value || std::is_floating_point<T2>::value ||
                                              std::is_floating_point<T3>::value>;

template<typename T1, typename T2, typename T3, class R = typename enable_if_some_float_t<T1, T2, T3>::type>
arr_real arange(T1 start, T2 stop, T3 step = 1) {
    const auto n = (int)(std::round((stop - start) / double(step)));
    arr_real r(n);
    for (int i = 0; i < n; ++i) {
        r[i] = start + (i * step);
    }
    return r;
}

inline arr_real arange(real_t stop) {
    return arange(real_t(0), stop, real_t(1));
}

inline arr_real arange(int start, int stop, int step = 1) {
    const auto n = (int)std::round((stop - start) / double(step));
    arr_real r(n);
    for (int i = 0; i < n; ++i) {
        r[i] = start;
        start += step;
    }
    return r;
}

inline arr_real arange(int stop) {
    return arange(0, stop, 1);
}

//-----------------------------------------------------------------------------------------------------------
template<typename T1, typename T2, typename T3, class R = typename enable_if_some_float_t<T1, T2, T3>::type>
[[deprecated("Use arange() instead.")]] arr_real range(T1 start, T2 stop, T3 step = 1) {
    return arange(start, stop, step);
}

[[deprecated("Use arange() instead.")]] inline arr_real range(real_t stop) {
    return arange(0.0, stop, 1.0);
}

[[deprecated("Use arange() instead.")]] inline arr_real range(int start, int stop, int step = 1) {
    return arange(start, stop, step);
}

[[deprecated("Use arange() instead.")]] inline arr_real range(int stop) {
    return arange(stop);
}

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
    return r;
}

inline arr_real ones(int n) {
    arr_real r(n);
    std::fill(r.begin(), r.end(), 1.0);
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

arr_real from_file(const std::string& file, dtype type = dtype::int16, endian order = endian::little, long offset = 0,
                   long count = std::numeric_limits<long>::max());

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
    static_assert(is_scalar_v<T>, "Type is not scalar");
    std::vector<T> res(arr.size());
    for (auto i = 0; i < arr.size(); i++) {
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
    for (auto i = 0; i < r.size(); i++) {
        r[i].re = *(p++);
        r[i].im = *(p++);
    }
    return r;
}

template<typename T>
inline arr_cmplx to_complex(const std::vector<T>& arr) {
    static_assert(is_scalar_v<T>, "Type is not scalar");
    return to_complex(arr.data(), arr.size());
}

template<typename T>
inline std::vector<T> from_complex(const arr_cmplx& arr) {
    static_assert(is_scalar_v<T>, "Type is not scalar");
    static_assert(std::is_convertible<real_t, T>::value, "Type is not convertible");
    std::vector<T> res(arr.size() * 2);
    for (auto i = 0; i < arr.size(); i++) {
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

//Estimate delay between signals
int finddelay(const dsplib::arr_real& x1, const dsplib::arr_real& x2);
int finddelay(const dsplib::arr_cmplx& x1, const dsplib::arr_cmplx& x2);

struct PeakList
{
    std::vector<real_t> pks;    ///< local maxima
    std::vector<real_t> locs;   ///< peak locations
    std::vector<real_t> wds;    ///< peak widths
};

// find local maxima
PeakList findpeaks(arr_real data, int npeaks);

// generate linearly spaced vector
arr_real linspace(real_t x1, real_t x2, size_t n);

// shift array circularly
arr_real circshift(const arr_real& x, int k);
arr_cmplx circshift(const arr_cmplx& x, int k);

//shift zero-frequency component to center of spectrum (swaps the left and right halves of X)
arr_real fftshift(const arr_real& x);
arr_cmplx fftshift(const arr_cmplx& x);

}   // namespace dsplib