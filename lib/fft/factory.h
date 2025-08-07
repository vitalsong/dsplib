#pragma once

#include "dsplib/fft.h"
#include "dsplib/ifft.h"

namespace dsplib::internal {

std::shared_ptr<FftPlanC> get_fft_plan(int n);
std::shared_ptr<FftPlanR> get_rfft_plan(int n);
std::shared_ptr<IfftPlanC> get_ifft_plan(int n);
std::shared_ptr<IfftPlanR> get_irfft_plan(int n);

}   // namespace dsplib::internal