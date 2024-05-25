#include <dsplib/types.h>
#include <cmath>
#include <limits>

namespace dsplib {

//TODO: constexpr

float eps(float v) {
    auto n = std::nextafter(v, std::numeric_limits<float>::infinity());
    return (n - v);
}

double eps(double v) {
    auto n = std::nextafter(v, std::numeric_limits<double>::infinity());
    return (n - v);
}

real_t eps() {
    return eps(real_t(1.0));
}

}   // namespace dsplib