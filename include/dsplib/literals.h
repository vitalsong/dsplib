#ifndef COMPLEX_LITERALS
#define COMPLEX_LITERALS

#pragma GCC system_header

#if (__cplusplus >= 201103L) & (__cplusplus < 201309)

#include <complex>

namespace std _GLIBCXX_VISIBILITY(default) {
inline namespace literals {
inline namespace complex_literals {
_GLIBCXX_BEGIN_NAMESPACE_VERSION

#define __cpp_lib_complex_udls 201309

  constexpr std::complex<float>
  operator""if(long double __num)
  { return std::complex<float>{0.0F, static_cast<float>(__num)}; }

  constexpr std::complex<float>
  operator""if(unsigned long long __num)
  { return std::complex<float>{0.0F, static_cast<float>(__num)}; }

  constexpr std::complex<double>
  operator""i(long double __num)
  { return std::complex<double>{0.0, static_cast<double>(__num)}; }

  constexpr std::complex<double>
  operator""i(unsigned long long __num)
  { return std::complex<double>{0.0, static_cast<double>(__num)}; }

  constexpr std::complex<long double>
  operator""il(long double __num)
  { return std::complex<long double>{0.0L, __num}; }

  constexpr std::complex<long double>
  operator""il(unsigned long long __num)
  { return std::complex<long double>{0.0L, static_cast<long double>(__num)}; }

_GLIBCXX_END_NAMESPACE_VERSION
} // inline namespace complex_literals
} // inline namespace literals
} // namespace

#endif  // C++11/14

#endif  /* COMPLEX_LITERALS */
