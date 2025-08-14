#pragma once

#include <dsplib.h>

namespace dsplib {

//Generalized cross-correlation

//TODO: store current corr peaks?
struct GccphatRes
{
    real_t tau;       ///< time delay
    arr_cmplx corr;   ///< cross-correlation between sig * refsig
};

GccphatRes gccphat(span_real sig, span_real refsig, int fs = 1);

struct MGccphatRes
{
    arr_real tau;                  ///< time delays
    std::vector<arr_cmplx> corr;   ///< cross-correlation between each pair: corr[k] = sig[k] * refsig
};

MGccphatRes gccphat(std::vector<span_real> sig, span_real refsig, int fs = 1);

}   // namespace dsplib