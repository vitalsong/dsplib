#pragma once

#include <dsplib/types.h>

#include <cstdint>
#include <vector>

namespace dsplib {

struct FftParam
{
    std::vector<real_t> coeffs;    ///[n] table of cos(2 * pi * i / n)
    std::vector<int32_t> bitrev;   ///[n/2] bitreverse index table
    uint32_t size;
};

//generate tables for radix 2 FFT
FftParam fft_tables(size_t size);

}   // namespace dsplib
