#include <dsplib/window.h>

#include <math.h>

namespace dsplib {
namespace window {

//-------------------------------------------------------------------------------------------------
arr_real rectangle(int n)
{
    arr_real arr(n);
    for (int i=0; i < n; ++i) {
        arr[i] = 1;
    }
    return arr;
}

//-------------------------------------------------------------------------------------------------
arr_real sinus(int n)
{
    arr_real arr(n);
    for (int i=0; i < n; ++i) {
        arr[i] = sin((pi * i) / (n - 1));
    }
    return arr;
}

//-------------------------------------------------------------------------------------------------
arr_real hann(int n)
{
    arr_real arr(n);
    for (int i=0; i < n; ++i) {
        arr[i] = 0.5 - 0.5 * cos((2 * pi * i) / (n - 1));
    }
    return arr;
}

//-------------------------------------------------------------------------------------------------
arr_real hamming(int n)
{
    arr_real arr(n);
    for (int i=0; i < n; ++i) {
        arr[i] = 0.54 - 0.46 * cos((2 * pi * i) / (n - 1));
    }
    return arr;
}

//-------------------------------------------------------------------------------------------------
arr_real blackman(int n)
{
    arr_real arr(n);
    for (int i=0; i < n; ++i) {
        arr[i] = 0.42 - 0.5 * cos((2 * pi * i) / (n - 1)) + 0.08 * cos((4 * pi * i) / (n - 1));
    }
    return arr;
}

//-------------------------------------------------------------------------------------------------
arr_real gauss(int n)
{
    arr_real arr(n);
    real_t q = 0.3;
    real_t a = (n - 1) / 2;
    for (int i=0; i < n; ++i) {
        arr[i] = exp((-0.5) * pow((i - a) / (q * a), 2));
    }
    return arr;
}

} ///< window
} ///< dsplib
