#pragma once

#include <dsplib/types.h>
#include <type_traits>

namespace dsplib {

template<typename T>
class base_array;

template<bool Cond_, typename Iftrue_, typename Iffalse_>
using conditional_t = typename std::conditional_t<Cond_, Iftrue_, Iffalse_>;

//reduce type for `a+b`, `a-b`, `a*b`, `a/b` operators
template<typename T1, typename T2>
constexpr auto reduce_operator_type() noexcept {
    using T1_ = std::remove_reference_t<T1>;
    using T2_ = std::remove_reference_t<T2>;
    if constexpr (is_scalar_v<T1_> && is_scalar_v<T2_>) {
        if constexpr (is_complex_v<T1_> || is_complex_v<T2_>) {
            return cmplx_t{};
        } else {
            return real_t{};
        }
    } else if constexpr (!is_scalar_v<T1_> && !is_scalar_v<T2_>) {
        using R1 = std::remove_reference_t<decltype(std::declval<T1_>()[0])>;
        using R2 = std::remove_reference_t<decltype(std::declval<T2_>()[0])>;
        return reduce_operator_type<R1, R2>();
    } else if constexpr (!is_scalar_v<T1_>) {
        using R1 = std::remove_reference_t<decltype(std::declval<T1_>()[0])>;
        return reduce_operator_type<R1, T2_>();
    } else {
        using R2 = std::remove_reference_t<decltype(std::declval<T2_>()[0])>;
        return reduce_operator_type<T1_, R2>();
    }
}

template<typename T1, typename T2>
using ResultType = decltype(reduce_operator_type<T1, T2>());

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