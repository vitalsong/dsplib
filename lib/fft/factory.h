#pragma once

#include <dsplib/fft.h>

namespace dsplib {

//instance or get cached fft plan

std::shared_ptr<BaseFftPlanC> create_fft_plan(int n);

std::shared_ptr<BaseFftPlanR> create_rfft_plan(int n);

}   // namespace dsplib