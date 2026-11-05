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

TEST(Throw, CompareArraySizes) {
    const std::vector<arr_real> arrays = {{}, {1, 2}, {1, 2, 3}};
    for (const auto& lhs : arrays) {
        for (const auto& rhs : arrays) {
            if (lhs.size() == rhs.size()) {
                continue;
            }
            EXPECT_THROW((void)(lhs > rhs), std::runtime_error);
            EXPECT_THROW((void)(lhs < rhs), std::runtime_error);
            EXPECT_THROW((void)(lhs == rhs), std::runtime_error);
            EXPECT_THROW((void)(lhs != rhs), std::runtime_error);
        }
    }
}

TEST(Throw, InvalidIndexVector) {
    const arr_real x = {1, 2, 3};
    const std::vector<std::vector<int>> indices = {{-1, 0}, {-1}, {3}, {0, 3}};
    for (const auto& idxs : indices) {
        EXPECT_THROW((void)x[idxs], std::runtime_error);
        EXPECT_THROW((void)x[arr_int(idxs)], std::runtime_error);
    }

    const arr_real empty;
    EXPECT_THROW((void)empty[std::vector<int>{0}], std::runtime_error);
    EXPECT_THROW((void)empty[arr_int{0}], std::runtime_error);
}
