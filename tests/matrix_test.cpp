#include "dsplib/matrix/matrix.h"
#include "dsplib/utils.h"
#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(Matrix, Base) {
    matrix_t<cmplx_t> m1 = {{1, 2}, {3, 4}, {5, 6}};
    ASSERT_EQ(m1.size()[0], 3);
    ASSERT_EQ(m1.size()[1], 2);

    matrix_t<real_t> m2 = {{1, 2, 3}, {4, 5, 6}};
    ASSERT_EQ(m2.size()[0], 2);
    ASSERT_EQ(m2.size()[1], 3);

    base_matrix_t<cmplx_t> r = m1 * m2;
    ASSERT_EQ(r.size()[0], 3);
    ASSERT_EQ(r.size()[1], 3);
}

//-------------------------------------------------------------------------------------------------
TEST(Matrix, CastToScalar) {
    matrix_t<real_t> p = {{11}};
    real_t r = p;
    ASSERT_EQ(r, 11);
}

//-------------------------------------------------------------------------------------------------
TEST(Matrix, CmplxCast) {
    matrix_t<real_t> rm = {{1, 2}, {3, 4}, {5, 6}};
    matrix_t<cmplx_t> cm = rm;
    ASSERT_EQ(cm.size()[0], 3);
    ASSERT_EQ(cm.size()[1], 2);
}

//-------------------------------------------------------------------------------------------------
TEST(Matrix, FromArray) {
    arr_real x = {1.0, 2.0, 3.0, 4.0};
    matrix_t<real_t> m(x);
    ASSERT_EQ(m.size()[0], x.size());
    ASSERT_EQ(m.size()[1], 1);
}