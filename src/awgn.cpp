#include <dsplib/awgn.h>
#include <dsplib/math.h>

#include <stdlib.h>
#include <math.h>
#include <random>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
arr_real awgn(const arr_real &arr, real_t snr)
{
    real_t stddev = rms(arr) * pow(10, ((-1) * snr / 20));

    arr_real r(arr);
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution <real_t> d(0, stddev);
    for (int i=0; i < r.size(); ++i) {
        r[i] += d(gen);
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx awgn(const arr_cmplx &arr, real_t snr)
{
    real_t stddev = 0.5 * rms(arr) * pow(10, ((-1) * snr / 20));

    arr_cmplx r(arr);
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution <real_t> d(0, stddev);
    for (int i=0; i < r.size(); ++i)
    {
        r[i].xi += d(gen);
        r[i].xq += d(gen);
    }

    return r;
}

}   ///< dsplib




