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

//join a sequence of arrays
//TODO: add slice args?
arr_real concatenate(span_real x1, span_real x2, span_real x3 = {}, span_real x4 = {}, span_real x5 = {});
arr_cmplx concatenate(span_cmplx x1, span_cmplx x2, span_cmplx x3 = {}, span_cmplx x4 = {}, span_cmplx x5 = {});

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
void flip(inplace_real x);
void flip(inplace_cmplx x);

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

//add zeros to the end of the array
arr_real zeropad(span_real x, int n);
arr_cmplx zeropad(span_cmplx x, int n);

//delays or advances the signal by the number of samples specified in delay
//TODO: fractional delay support
template<typename T>
base_array<T> delayseq(const base_array<T>& data, int delay) {
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
real_t peakloc(span_real x, int idx, bool cyclic = true);
real_t peakloc(span_cmplx x, int idx, bool cyclic = true);

//Estimate delay between signals
int finddelay(span_real x1, span_real x2);
int finddelay(span_cmplx x1, span_cmplx x2);

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

/**
 * @brief generate Zadoff-Chu sequence
 * @param r root, from 1 to n-1
 * @param n length
 * @return Zadoff-Chu sequence
 */
arr_cmplx zadoff_chu(int r, int n);

}   // namespace dsplib