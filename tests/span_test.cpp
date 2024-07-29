#include "tests_common.h"
#include <gtest/gtest.h>

using namespace dsplib;

static real_t _first_span_elem(const span_real& x) {
    return x[0];
}

TEST(SpanTest, Base) {
    {
        arr_real x1 = {-1, 2, -3, 4, -5};
        x1.slice(0, 2) = 100;
        ASSERT_EQ_ARR_REAL(x1, arr_real{100, 100, -3, 4, -5});
    }
}

TEST(SpanTest, ConstMutable) {
    {
        arr_real x1 = {-1, 1, 2, 3};
        auto span = x1.slice(0, indexing::end);
        ASSERT_EQ(span.size(), 4);
        arr_real x2(span);
        ASSERT_EQ_ARR_REAL(x1, x2);
        ASSERT_EQ(_first_span_elem(span), -1);
        ASSERT_EQ(_first_span_elem(x1), -1);
    }

    {
        arr_real x1 = {-1, 2, -3, 4, -5};
        auto span = x1.slice(1, 3);
        ASSERT_EQ(span.size(), 2);
        arr_real x2(span);
        ASSERT_EQ_ARR_REAL(x2, arr_real{2, -3});
        ASSERT_EQ(_first_span_elem(span), 2);
    }
}

TEST(SpanTest, StdVector) {
    {
        std::vector<real_t> x = {-1, 1, 2, 3};
        ASSERT_EQ(dsplib::max(x), x[3]);
    }
    {
        std::vector<cmplx_t> x = {-1i, 1i, 2i, 3i};
        ASSERT_CMPLX_EQ(dsplib::max(x), x[3]);
    }
}