#include <dsplib/allocator.h>
#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(Allocator, Alloc) {
    pool_reset();

    auto x1 = pool_alloc(100);
    ASSERT_EQ(pool_info().size(), 1);
    ASSERT_EQ(pool_info().at(0).used, true);
    ASSERT_EQ(pool_info().at(0).size, 512);
    pool_free(x1);

    auto x2 = pool_alloc(120);
    ASSERT_EQ(pool_info().size(), 1);
    ASSERT_EQ(pool_info().at(0).used, true);
    ASSERT_EQ(pool_info().at(0).size, 512);
    pool_free(x2);

    ASSERT_TRUE(x1 == x2);
    ASSERT_EQ(pool_info().size(), 1);
    ASSERT_EQ(pool_info().at(0).used, false);
    ASSERT_EQ(pool_info().at(0).size, 512);
}

//-------------------------------------------------------------------------------------------------
TEST(Allocator, Vector) {
    pool_reset();
    uintptr_t p1, p2;

    {
        std::vector<double, pool_allocator<double>> x1(10);
        x1[0] = 0;
        p1 = uintptr_t(x1.data());
        x1.resize(0);
        x1.shrink_to_fit();
    }

    {
        std::vector<double, pool_allocator<double>> x2(10);
        x2[0] = 1;
        p2 = uintptr_t(x2.data());
        x2.resize(0);
        x2.shrink_to_fit();
    }

    ASSERT_TRUE(p1 == p2);
    ASSERT_EQ(pool_info().size(), 1);
    ASSERT_EQ(pool_info().at(0).used, false);
    ASSERT_EQ(pool_info().at(0).size, 512);
}
