#pragma once

#include <dsplib/array.h>

#include <vector>
#include <memory>
#include <cstdint>

namespace dsplib {
namespace tables {

class fft2tb;
using fft2tb_ptr = std::shared_ptr<fft2tb>;

//wrapper for table exp(-1i * 2 * pi * i / n) compresed to 1/4
class fft2tb
{
public:
    static fft2tb_ptr alloc(size_t n);
    static void reset(size_t n);
    static bool is_cached(size_t n);

    arr_cmplx unpack() const noexcept;

private:
    explicit fft2tb(uint32_t n) noexcept;

    const uint32_t _n;
    const uint32_t _n2;
    const uint32_t _n4;
    std::vector<real_t> _cos_tb;
};

//bit-reverse table
using bitrev_ptr = std::shared_ptr<std::vector<int32_t>>;
bitrev_ptr bitrev_table(size_t n);
bool bitrev_cached(size_t n);
void bitrev_clear(size_t n);

}   // namespace tables
}   // namespace dsplib
