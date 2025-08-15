#include "tests_common.h"

#include <dsplib.h>

using namespace dsplib;

TEST(Buffer, ChunkBufferTest) {
    const int chunk_size = 100;
    auto x = randn(chunk_size * 1000 + chunk_size - 1);
    auto buf = ChunkBuffer<real_t>(chunk_size);

    arr_real y;
    buf.write(x, [&](auto v) {
        ASSERT_TRUE(v.size() == chunk_size);
        y = concatenate(y, v);
    });

    ASSERT_TRUE(x.size() != y.size());

    ASSERT_EQ_ARR_REAL(x.slice(0, y.size()), y);

    ASSERT_TRUE(buf.size() == chunk_size - 1);

    buf.flush([&](auto v) {
        ASSERT_TRUE(v.size() == chunk_size - 1);
        y = concatenate(y, v);
    });

    ASSERT_EQ_ARR_REAL(x, y);

    buf.flush([&](auto v) {
        ASSERT_TRUE(v.empty());
    });

    int ncall = 0;
    buf.write(zeros(2 * chunk_size), [&](auto v) {
        ASSERT_EQ_ARR_REAL(v, zeros(chunk_size));
        ++ncall;
    });
    ASSERT_TRUE(ncall == 2);
}

TEST(Buffer, FIFOBufferTest) {
    FIFOBuffer<real_t> buf;
    {
        auto x = randn(1000);
        buf.write(x);
        ASSERT_TRUE(buf.size() == 1000);
        auto y = buf.read(1000);
        ASSERT_TRUE(buf.empty());
        ASSERT_EQ_ARR_REAL(x, y);
    }
    {
        auto x = randn(512);
        auto y = arr_real(512);
        buf.write(x);
        buf.read(make_span(y));
        ASSERT_TRUE(buf.empty());
        ASSERT_EQ_ARR_REAL(x, y);
    }
    {
        auto x = randn(512);
        buf.write(x);
        arr_real y1 = buf.read(256);
        arr_real y2 = buf.read(256);
        auto y = y1 | y2;
        ASSERT_TRUE(buf.empty());
        ASSERT_EQ_ARR_REAL(x, y);
    }
    {
        auto x = randn(512);
        buf.write(x);
        ASSERT_TRUE(buf.size() == 512);
        auto y = buf.view();
        ASSERT_EQ_ARR_REAL(x, y);
        buf.reset();
        ASSERT_TRUE(buf.empty());
    }
}
