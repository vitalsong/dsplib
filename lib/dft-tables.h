#pragma once

#include <dsplib/array.h>

#include <vector>
#include <stdint.h>

namespace dsplib {
namespace tables {

constexpr int DEFAULT_MIN_NFFT = 512;

// table for calculating DFT
dsplib::arr_cmplx dft_table(size_t n);

// bit-reverse table
std::vector<int32_t> bitrev_table(size_t n);

}   // namespace tables
}   // namespace dsplib
