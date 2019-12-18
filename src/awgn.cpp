#include <dsplib/awgn.h>
#include <dsplib/math.h>

#include <stdlib.h>
#include <math.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
arr_real awgn(const arr_real &arr, real_t snr)
{
    const int N = arr.size();
    real_t signal = rms(arr);
    real_t noise = signal * pow(10, ((-1) * snr / 20));
    real_t stddev = noise;

    arr_real r(N);
    const int M = 12;
    for (int i=0; i < N; ++i)
    {
        real_t sum = 0;
        for (int j=0; j < M; ++j) {
            sum += rand();
        }

        r[i] = arr[i] + (stddev * (sum/RAND_MAX - M/2));
    }

    return r;
}

//-------------------------------------------------------------------------------------------------
arr_cmplx awgn(const arr_cmplx &arr, real_t snr)
{
    const int N = arr.size();
    real_t signal = rms(arr)/2;
    real_t noise = signal * pow(10, ((-1) * snr / 20));
    real_t stddev = noise;

    arr_cmplx r(N);
    const int M = 12;
    for (int i=0; i < N; ++i)
    {
        real_t sum = 0;
        for (int j=0; j < M; ++j) {
            sum += rand();
        }

        r[i].xi = arr[i].xi + (stddev * (sum/RAND_MAX - M/2));
        r[i].xq = arr[i].xq + (stddev * (sum/RAND_MAX - M/2));
    }

    return r;
}

}   ///< dsplib




