#include <dsplib/array.h>
#include <iostream>

namespace dsplib {

template<typename T>
std::ostream& operator<<(std::ostream& os, const base_array<T>& x) {
    std::for_each(x.begin(), x.end() - 1, [&](auto v) {
        os << v << ", ";
    });
    return os << *(x.end() - 1);
}

std::ostream& operator<<(std::ostream& os, const cmplx_t& x) {
    const std::string sign = (x.im >= 0) ? "+" : "-";
    return os << x.re << sign << std::abs(x.im) << "i";
}

}   // namespace dsplib