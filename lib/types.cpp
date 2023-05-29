#include <dsplib/types.h>
#include <cmath>

namespace dsplib {

double eps(float v) {
    auto n = std::nextafter(v, INFINITY);
    return (n - v);
}

double eps(double v) {
    auto n = std::nextafter(v, INFINITY);
    return (n - v);
}

real_t eps() {
    return eps(real_t(1.0));
}

}   // namespace dsplib