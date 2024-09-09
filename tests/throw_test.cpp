#include "tests_common.h"
#include <gtest/gtest.h>

using namespace dsplib;

TEST(Throw, BadArraySize) {
    auto fn = [] {
        arr_real x1 = {1.0, 2.0, 3.0};
        arr_real x2 = {1.0, 2.0};
        auto y = x1 + x2;
    };
    EXPECT_THROW({ fn(); }, std::runtime_error);
}

TEST(Throw, IndexOutRange) {
    auto fn = [] {
        arr_real x = {1.0, 2.0, 3.0};
        auto y = x[std::vector<int>{0, 1, 2, 3}];
    };
    EXPECT_THROW({ fn(); }, std::runtime_error);
}