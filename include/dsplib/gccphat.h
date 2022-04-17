#pragma once

#include <dsplib.h>

namespace dsplib {

//Generalized cross-correlation
real_t gccphat(const arr_real& sig, const arr_real& refsig, int fs = 1);
arr_real gccphat(const std::vector<arr_real>& sig, const arr_real& refsig, int fs = 1);

}   // namespace dsplib