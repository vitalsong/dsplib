#pragma once

#include <memory>
#include <dsplib/fft.h>

namespace dsplib::specific {

std::shared_ptr<BaseFftPlanC> get_fft_plan(int n);

std::shared_ptr<BaseFftPlanR> get_rfft_plan(int n);

}   // namespace dsplib::specific