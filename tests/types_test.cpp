#include <dsplib.h>
#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(Template, CmplxConvert) {
    //scalar -> cmplx_t
    ASSERT_TRUE(bool(std::is_convertible<double, cmplx_t>::value));
    ASSERT_TRUE(bool(std::is_convertible<cmplx_t, cmplx_t>::value));
    ASSERT_TRUE(bool(std::is_convertible<real_t, cmplx_t>::value));
    ASSERT_TRUE(bool(std::is_convertible<int, cmplx_t>::value));
    ASSERT_TRUE(bool(std::is_convertible<float, cmplx_t>::value));

    //std::complex<T> -> cmplx_t
    ASSERT_TRUE(bool(std::is_convertible<std::complex<double>, cmplx_t>::value));
    ASSERT_TRUE(bool(std::is_convertible<std::complex<int>, cmplx_t>::value));
    ASSERT_TRUE(bool(std::is_convertible<std::complex<float>, cmplx_t>::value));

    //cmplx_t -> std::complex<T>
    ASSERT_TRUE(bool(std::is_convertible<cmplx_t, std::complex<double>>::value));
    ASSERT_TRUE(bool(std::is_convertible<cmplx_t, std::complex<float>>::value));

    //cmplx_t -> scalar
    ASSERT_FALSE(bool(std::is_convertible<cmplx_t, double>::value));
    ASSERT_FALSE(bool(std::is_convertible<cmplx_t, real_t>::value));
    ASSERT_FALSE(bool(std::is_convertible<cmplx_t, int>::value));
    ASSERT_FALSE(bool(std::is_convertible<cmplx_t, float>::value));
}

//-------------------------------------------------------------------------------------------------
TEST(Template, IsScalar) {
    ASSERT_TRUE(bool(is_scalar_ar<cmplx_t>::value));
    ASSERT_TRUE(bool(is_scalar_ar<real_t>::value));
}
