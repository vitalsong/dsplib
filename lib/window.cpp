#include <dsplib/window.h>

#include <cmath>

namespace dsplib {
namespace window {

//-------------------------------------------------------------------------------------------------
arr_real rectangle(int n) {
    arr_real arr(n);
    for (int i = 0; i < n; ++i) {
        arr[i] = 1;
    }
    return arr;
}

//-------------------------------------------------------------------------------------------------
arr_real sinus(int n) {
    arr_real arr(n);
    for (int i = 0; i < n; ++i) {
        arr[i] = std::sin((pi * i) / (n - 1));
    }
    return arr;
}

//-------------------------------------------------------------------------------------------------
arr_real hann(int n) {
    arr_real arr(n);
    for (int i = 0; i < n; ++i) {
        arr[i] = 0.5 - 0.5 * std::cos((2 * pi * i) / (n - 1));
    }
    return arr;
}

//-------------------------------------------------------------------------------------------------
arr_real hamming(int n) {
    arr_real arr(n);
    for (int i = 0; i < n; ++i) {
        arr[i] = 0.54 - 0.46 * std::cos((2 * pi * i) / (n - 1));
    }
    return arr;
}

//-------------------------------------------------------------------------------------------------
arr_real blackman(int n) {
    arr_real arr(n);
    for (int i = 0; i < n; ++i) {
        arr[i] = 0.42 - 0.5 * std::cos((2 * pi * i) / (n - 1)) + 0.08 * std::cos((4 * pi * i) / (n - 1));
    }
    return arr;
}

//-------------------------------------------------------------------------------------------------
arr_real gauss(int n) {
    arr_real arr(n);
    real_t q = 0.3;
    real_t a = (n - 1) / 2;
    for (int i = 0; i < n; ++i) {
        arr[i] = std::exp((-0.5) * std::pow((i - a) / (q * a), 2));
    }
    return arr;
}

}   // namespace window
}   // namespace dsplib
