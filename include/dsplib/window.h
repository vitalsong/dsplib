#pragma once

#include <dsplib/array.h>

namespace dsplib {
namespace window {

arr_real rectangle(int n);
arr_real sinus(int n);
arr_real hann(int n);
arr_real hamming(int n);
arr_real blackman(int n);
arr_real gauss(int n, real_t alpha = 2.5);
arr_real blackmanharris(int n, bool periodic = false);

}   // namespace window
}   // namespace dsplib
