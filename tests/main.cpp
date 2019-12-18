#include "arr_real_test.h"
#include "arr_cmpl_test.h"
#include "math_test.h"

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
