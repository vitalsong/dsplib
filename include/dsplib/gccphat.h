#pragma once

#include <dsplib.h>

namespace dsplib {

//Generalized cross-correlation

//TODO: store current corr peaks?
struct gccphat_res_t
{
    real_t tau;       ///< time delay
    arr_cmplx corr;   ///< cross-correlation between sig * refsig
};

gccphat_res_t gccphat(const arr_real& sig, const arr_real& refsig, int fs = 1);

struct gccphat_res_ch_t
{
    arr_real tau;                  ///< time delays
    std::vector<arr_cmplx> corr;   ///< cross-correlation between each pair: corr[k] = sig[k] * refsig
};

gccphat_res_ch_t gccphat(const std::vector<arr_real>& sig, const arr_real& refsig, int fs = 1);

}   // namespace dsplib