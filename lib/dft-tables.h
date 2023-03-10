#pragma once

#include <dsplib/array.h>
#include <vector>
#include <stdint.h>

namespace dsplib {

struct FFTParam
{
    arr_cmplx coeffs;
    std::vector<int32_t> bitrev;
};

FFTParam fft_tables(size_t size);

}   // namespace dsplib
