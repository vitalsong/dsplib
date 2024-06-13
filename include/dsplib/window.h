#pragma once

#include <dsplib/array.h>

namespace dsplib {
namespace window {

arr_real cosine(int n, bool sym = true);

arr_real hann(int n, bool sym = true);

arr_real hamming(int n, bool sym = true);

arr_real blackman(int n, bool sym = true);

arr_real gauss(int n, real_t alpha = 2.5, bool sym = true);

arr_real blackmanharris(int n, bool sym = true);

arr_real kaiser(int n, real_t beta = 0.5);

arr_real tukey(int n, real_t r = 0.5);

}   // namespace window
}   // namespace dsplib
