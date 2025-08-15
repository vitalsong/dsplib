#pragma once

#include <dsplib/types.h>
#include <type_traits>

namespace dsplib {

template<typename T>
class base_array;

template<bool Cond_, typename Iftrue_, typename Iffalse_>
using conditional_t = typename std::conditional_t<Cond_, Iftrue_, Iffalse_>;

//Detect out type for operations:
//real + real -> real
//cmplx + cmplx -> cmplx
//real + cmplx -> cmplx
//cmplx + real -> cmplx
template<typename T1, typename T2>
using ResultType = conditional_t<std::is_same_v<T1, cmplx_t> || std::is_same_v<T2, cmplx_t>, cmplx_t, real_t>;

template<typename T_dst, typename T_src>
constexpr base_array<T_dst> array_cast(const base_array<T_src>& src) noexcept {
    static_assert(is_scalar_v<T_src> && is_scalar_v<T_dst>, "Types must be scalar");
    static_assert(!(is_complex_v<T_src> && !is_complex_v<T_dst>), "Complex to real cast is not allowed");
    if constexpr (std::is_same_v<T_src, T_dst>) {
        return src;
    } else if constexpr (!is_complex_v<T_src> && is_complex_v<T_dst>) {
        base_array<T_dst> dst(src.size());
        for (int i = 0; i < src.size(); ++i) {
            dst[i].re = static_cast<real_t>(src[i]);
        }
        return dst;
    } else {
        return base_array<T_dst>(src);
    }
}

//rules for implicit array conversion
//TODO: use static_assert and verbose error message
template<typename T_src, typename T_dst>
constexpr bool is_array_convertible() noexcept {
    if constexpr (!std::is_convertible_v<T_src, T_dst>) {
        return false;
    }

    //only arithmetic scalar
    if constexpr (!is_scalar_v<T_src> || !is_scalar_v<T_dst>) {
        return false;
    }

    //cmplx -> real
    if constexpr (is_complex_v<T_src> && !is_complex_v<T_dst>) {
        return false;
    }

    //real -> cmplx
    if constexpr (!is_complex_v<T_src> && is_complex_v<T_dst>) {
        return false;
    }

    //float -> int
    if constexpr (std::is_floating_point_v<T_src> && std::is_integral_v<T_dst>) {
        return false;
    }

    return true;
}

template<typename T>
constexpr bool support_type_for_array() {
    using U = std::remove_cv_t<T>;
    if constexpr (std::is_same_v<U, real_t>) {
        return true;
    }
    if constexpr (std::is_same_v<U, cmplx_t>) {
        return true;
    }
    if constexpr (std::is_same_v<U, int>) {
        return true;
    }
    return false;
}

}   // namespace dsplib