#pragma once

#include <dsplib/array.h>
#include <cstdint>

namespace dsplib {

struct FftParam
{
    dsplib::arr_cmplx coeffs;
    std::vector<int32_t> bitrev;
};

//generate tables for radix 2 FFT
FftParam fft_tables(size_t size);

}   // namespace dsplib
