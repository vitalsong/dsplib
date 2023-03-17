#include <dsplib/array.h>
#include <ostream>

namespace dsplib {

template<typename T>
static std::ostream& format(std::ostream& os, const base_array<T>& x) {
    for (int i = 0; i < (x.size() - 1); ++i) {
        os << x[i] << ", ";
    }
    return os << x[x.size() - 1];
}

template<>
std::ostream& base_array<real_t>::_print(std::ostream& os) const {
    return format(os, *this);
}

template<>
std::ostream& base_array<cmplx_t>::_print(std::ostream& os) const {
    return format(os, *this);
}

std::ostream& operator<<(std::ostream& os, const cmplx_t& x) {
    const std::string sign = (x.im >= 0) ? "+" : "-";
    return os << x.re << sign << std::abs(x.im) << "i";
}

}   // namespace dsplib