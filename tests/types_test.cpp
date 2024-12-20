#include <dsplib.h>
#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(Template, CmplxScalarConvert) {
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
    ASSERT_TRUE(bool(is_scalar_v<cmplx_t>));
    ASSERT_TRUE(bool(is_scalar_v<real_t>));
}

//-------------------------------------------------------------------------------------------------
TEST(Template, ArrayConvert) {
    {
        ASSERT_TRUE(bool(is_array_convertible<float, double>()));
        dsplib::base_array<float> x(dsplib::base_array<double>(10));
    }

    {
        ASSERT_TRUE(bool(is_array_convertible<double, float>()));
        dsplib::base_array<double> x(dsplib::base_array<float>(10));
    }

    {
        ASSERT_FALSE(bool(is_array_convertible<float, int>()));
        ASSERT_TRUE(bool(is_array_convertible<int, float>()));
        dsplib::base_array<float> x(dsplib::base_array<int>(10));
    }

    {
        ASSERT_TRUE(bool(is_array_convertible<std::complex<real_t>, cmplx_t>()));
        dsplib::base_array<cmplx_t> x(dsplib::base_array<std::complex<real_t>>(10));
    }

    ASSERT_FALSE(bool(is_array_convertible<real_t, cmplx_t>()));
    ASSERT_FALSE(bool(is_array_convertible<cmplx_t, real_t>()));
}