#include <dsplib/allocator.h>
#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(Allocator, EqualPointer) {
    {
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

    {
        pool_reset();
        double *p1, *p2;

        {
            std::vector<double, pool_allocator<double>> x(100);
            p1 = x.data();
        }

        {
            std::vector<double, pool_allocator<double>> x(102);
            p2 = x.data();
        }

        ASSERT_TRUE(p1 == p2);
        ASSERT_EQ(pool_info().size(), 1);
        ASSERT_EQ(pool_info().at(0).used, false);
        ASSERT_EQ(pool_info().at(0).size, 1024);
    }
}
