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

inline dsplib::arr_real repelem(const dsplib::arr_real& x, int n) {
    return _repelem<dsplib::arr_real>(x, n);
}

inline dsplib::arr_cmplx repelem(const dsplib::arr_cmplx& x, int n) {
    return _repelem<dsplib::arr_cmplx>(x, n);
}

}   // namespace dsplib