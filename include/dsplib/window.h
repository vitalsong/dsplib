#ifndef WINDOW_H
#define WINDOW_H

#include <dsplib/array.h>

namespace dsplib {
namespace window {

arr_real rectangle(int n);
arr_real sinus(int n);
arr_real hann(int n);
arr_real hamming(int n);
arr_real blackman(int n);
arr_real gauss(int n);

} ///< window
} ///< dsplib

#endif // WINDOW_H
