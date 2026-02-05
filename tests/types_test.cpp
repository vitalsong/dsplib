#include <dsplib.h>
#include "tests_common.h"

using namespace dsplib;

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

TEST(Template, IsScalar) {
    ASSERT_TRUE(bool(is_scalar_v<cmplx_t>));
    ASSERT_TRUE(bool(is_scalar_v<real_t>));
}

TEST(Template, ArrayConvert) {
    {
        ASSERT_TRUE(bool(is_array_convertible<float, real_t>()));
        dsplib::base_array<real_t> x(std::vector<float>(10));
    }

    {
        ASSERT_TRUE(bool(is_array_convertible<double, real_t>()));
        dsplib::base_array<real_t> x(std::vector<double>(10));
    }

    {
        ASSERT_FALSE(bool(is_array_convertible<real_t, int>()));
        ASSERT_TRUE(bool(is_array_convertible<int, real_t>()));
        dsplib::base_array<real_t> x(std::vector<int>(10));
    }

    {
        ASSERT_TRUE(bool(is_array_convertible<std::complex<real_t>, cmplx_t>()));
        dsplib::base_array<cmplx_t> x(std::vector<std::complex<real_t>>(10));
    }

    ASSERT_FALSE(bool(is_array_convertible<real_t, cmplx_t>()));
    ASSERT_FALSE(bool(is_array_convertible<cmplx_t, real_t>()));
}

TEST(Template, SupportTypeArray) {
    ASSERT_TRUE(support_type_for_array<real_t>());
    ASSERT_TRUE(support_type_for_array<cmplx_t>());
    ASSERT_TRUE(support_type_for_array<int>());
    ASSERT_FALSE(support_type_for_array<std::complex<float>>());
    ASSERT_FALSE(support_type_for_array<std::complex<double>>());
}

TEST(Template, ResultType) {
    ASSERT_TRUE(bool(std::is_same_v<ResultType<real_t, real_t>, real_t>));
    ASSERT_TRUE(bool(std::is_same_v<ResultType<real_t, float>, real_t>));
    ASSERT_TRUE(bool(std::is_same_v<ResultType<float, real_t>, real_t>));
    ASSERT_TRUE(bool(std::is_same_v<ResultType<real_t, int>, real_t>));
    ASSERT_TRUE(bool(std::is_same_v<ResultType<real_t, double>, real_t>));

    ASSERT_TRUE(bool(std::is_same_v<ResultType<cmplx_t, real_t>, cmplx_t>));
    ASSERT_TRUE(bool(std::is_same_v<ResultType<cmplx_t, float>, cmplx_t>));
    ASSERT_TRUE(bool(std::is_same_v<ResultType<float, cmplx_t>, cmplx_t>));
    ASSERT_TRUE(bool(std::is_same_v<ResultType<cmplx_t, int>, cmplx_t>));
    ASSERT_TRUE(bool(std::is_same_v<ResultType<cmplx_t, double>, cmplx_t>));
    ASSERT_TRUE(bool(std::is_same_v<ResultType<float, std::complex<float>>, cmplx_t>));
}
