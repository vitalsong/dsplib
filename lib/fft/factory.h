#pragma once

#include <dsplib/ifft.h>
#include <dsplib/fft.h>

namespace dsplib {

//instance or get cached fft plan

//FFT
std::shared_ptr<BaseFftPlanC> create_fft_plan(int n);
std::shared_ptr<BaseFftPlanR> create_rfft_plan(int n);

//IFFT
std::shared_ptr<BaseIfftPlanC> create_ifft_plan(int n);
std::shared_ptr<BaseIfftPlanR> create_irfft_plan(int n);

}   // namespace dsplib