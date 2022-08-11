#include <dsplib/allocator.h>
#include "tests_common.h"

using namespace dsplib;

//-------------------------------------------------------------------------------------------------
TEST(Allocator, EqualPointer) {
    {
        auto x1 = pool_alloc(100);
        pool_free(x1);
        auto x2 = pool_alloc(120);
        pool_free(x2);
        ASSERT_TRUE(x1 == x2);
    }

    {
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
    }
}
