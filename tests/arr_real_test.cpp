#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, Init) {
    arr_real a1;
    arr_real a3(a1);
    arr_real a4(10);
    arr_real a5 = a4;
    arr_real a6 = a5 + a4;
    arr_real a7(a6 + a4);

    ASSERT_EQ(a1.size(), 0);
    ASSERT_EQ(a3.size(), 0);
    ASSERT_EQ(a4.size(), 10);
    ASSERT_EQ(a5.size(), 10);
    ASSERT_EQ(a6.size(), 10);
    ASSERT_EQ(a7.size(), 10);
    ASSERT_TRUE(a1.empty());
    ASSERT_TRUE(a3.empty());

    std::vector<short> v1 = {-1, -2, 3, 4};
    arr_real a8{v1.data(), v1.size()};
    ASSERT_EQ_ARR_REAL(v1, a8);

    std::vector<::real_t> v2 = {1, 2, 3, 4};
    arr_real a9(v2);
    ASSERT_EQ_ARR_REAL(v2, a9);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, Cast) {
    {
        const arr_int x1 = {1, 2, 3, 4, 5};
        const arr_real x2(x1);
        ASSERT_EQ_ARR_REAL(x1, x2);
    }
    {
        arr_real x1 = {1, 2, 3, 4, 5};
        x1 = x1;
        ASSERT_EQ_ARR_REAL(x1, arr_real{1, 2, 3, 4, 5});
    }
    {
        arr_real x1 = {1, 2, 3, 4, 5};
        arr_real x2 = std::move(x1);
        ASSERT_EQ_ARR_REAL(x2, arr_real{1, 2, 3, 4, 5});
        ASSERT_EQ(x1.size(), 0);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, Arithm) {
    arr_real a1 = {-1, -2, 3, 4};
    arr_real a2 = {1, 2, -3, -4};

    arr_real sum_r = {0, 0, 0, 0};
    arr_real mlt_r = {-1, -4, -9, -16};
    arr_real div_r = {-1, -1, -1, -1};

    auto sum = a1 + a2;
    auto mlt = a1 * a2;
    auto div = a1 / a2;

    ASSERT_EQ_ARR_REAL(sum, sum_r);
    ASSERT_EQ_ARR_REAL(mlt, mlt_r);
    ASSERT_EQ_ARR_REAL(div, div_r);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, Concatenate) {
    arr_real a1 = {100, 200};
    arr_real a2 = {-100, -200};
    arr_real j1 = {100, 200, -100, -200};
    arr_real j2 = {-100, -200, 100, 200};

    auto r1 = concatenate(a1, a2);
    auto r2 = concatenate(a2, a1);
    ASSERT_EQ_ARR_REAL(r1, j1);
    ASSERT_EQ_ARR_REAL(r2, j2);

    ASSERT_EQ_ARR_REAL(a1 | a2, j1);
    ASSERT_EQ_ARR_REAL(a2 | a1, j2);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, Zeros) {
    arr_real r = {0, 0, 0, 0};
    auto v = zeros(4);
    ASSERT_EQ_ARR_REAL(r, v);
}

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, Power) {
    {
        arr_real x1 = {1, -1, 2, -2};
        arr_real r1 = {1, 1, 4, 4};
        auto y1 = power(x1, 2);
        ASSERT_EQ_ARR_REAL(r1, y1);
    }

    {
        arr_real x1 = {1, -2.0, 3, -4.0};
        arr_real r1 = {1, 1, 1, 1};
        auto y1 = power(x1, 0);
        ASSERT_EQ_ARR_REAL(r1, y1);
    }

    {
        arr_real x1 = {1, 2, 3, 4};
        arr_real x2 = {2, 2, 2, 2};
        arr_real r = {1, 4, 9, 16};
        ASSERT_EQ_ARR_REAL(power(x1, x2), r);
    }

    {
        real_t x1 = 2;
        arr_real x2 = {1, 2, 3, 4};
        arr_real r = {2, 4, 8, 16};
        ASSERT_EQ_ARR_REAL(power(x1, x2), r);
    }
}

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, CheckEqual) {
    const arr_real x = {1.0, -1.0, 200, -1e-5, 0.0};
    {
        std::vector<bool> r = (x == (x[0] + 1e-17));
        ASSERT_TRUE(bool(r == std::vector<bool>{true, false, false, false, false}));
    }
    {
        std::vector<bool> r = (x == (x[1] + 1e-17));
        ASSERT_TRUE(bool(r == std::vector<bool>{false, true, false, false, false}));
    }
    {
        std::vector<bool> r = (x == (x[2] + 1e-17));
        ASSERT_TRUE(bool(r == std::vector<bool>{false, false, true, false, false}));
    }
    {
        std::vector<bool> r = (x == (x[3] + 1e-23));
        ASSERT_TRUE(bool(r == std::vector<bool>{false, false, false, true, false}));
    }
    {
        std::vector<bool> r = (x == (x[4] + 1e-30));
        ASSERT_TRUE(bool(r == std::vector<bool>{false, false, false, false, false}));
    }
    {
        std::vector<bool> r = (x == -0.0);
        ASSERT_TRUE(bool(r == std::vector<bool>{false, false, false, false, true}));
    }
}

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, CompareScalar) {
    const arr_real x = {1.0, -1.0, 200, -1e-5, 10000.0};
    {
        std::vector<bool> r = (x > 1.0);
        ASSERT_TRUE(bool(r == std::vector<bool>{false, false, true, false, true}));
        ASSERT_EQ_ARR_REAL(x[r], arr_real{200, 10000.0});
    }

    {
        std::vector<bool> r = (x < 1.0);
        ASSERT_TRUE(bool(r == std::vector<bool>{false, true, false, true, false}));
        ASSERT_EQ_ARR_REAL(x[r], arr_real{-1.0, -1e-5});
    }

    {
        std::vector<bool> r = (abs(x) == 1.0);
        ASSERT_TRUE(bool(r == std::vector<bool>{true, true, false, false, false}));
        ASSERT_EQ_ARR_REAL(x[r], arr_real{1.0, -1.0});
    }
}

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, CompareVector) {
    const arr_real x = {1.0, -1.0, 200, 1e-5, 10000.0};
    const arr_real y = {1.0, 2.0, 300, 1e-6, 10001.0};
    {
        std::vector<bool> r = (x > y);
        ASSERT_TRUE(bool(r == std::vector<bool>{false, false, false, true, false}));
        ASSERT_EQ_ARR_REAL(x[r], arr_real{1e-5});
    }

    {
        std::vector<bool> r = (x < y);
        ASSERT_TRUE(bool(r == std::vector<bool>{false, true, true, false, true}));
        ASSERT_EQ_ARR_REAL(x[r], arr_real{-1.0, 200, 10000.0});
    }

    {
        std::vector<bool> r = (x == y);
        ASSERT_TRUE(bool(r == std::vector<bool>{true, false, false, false, false}));
        ASSERT_EQ_ARR_REAL(x[r], arr_real{1.0});
    }
}

//-------------------------------------------------------------------------------------------------
TEST(ArrRealTest, UnaryOperators) {
    {
        const auto x1 = dsplib::arange(32);
        auto x2 = -x1;
        auto x3 = +x1;
        ASSERT_EQ_ARR_REAL(x1, dsplib::arange(32));
        ASSERT_EQ_ARR_REAL(x2, -dsplib::arange(32));
        ASSERT_EQ_ARR_REAL(x3, dsplib::arange(32));
    }
    {
        auto x1 = dsplib::arange(32);
        auto x2 = -x1;
        auto x3 = +x1;
        ASSERT_EQ_ARR_REAL(x1, dsplib::arange(32));
        ASSERT_EQ_ARR_REAL(x2, -dsplib::arange(32));
        ASSERT_EQ_ARR_REAL(x3, dsplib::arange(32));
    }
}