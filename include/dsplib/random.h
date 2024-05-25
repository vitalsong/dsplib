#include <dsplib/array.h>

#include <array>

namespace dsplib {

//control random number generator
//each thread has its own seed for generation
void rng(int seed);

//note: can be slow for every call
//randi(n) faster than loop(n).randi()

//integer uniform [1, imax]
int randi(int imax);
arr_int randi(int imax, int n);

//integer uniform [imin, imax]
arr_int randi(std::array<int, 2> range, int n);
int randi(std::array<int, 2> range);

//uniformly distributed random numbers
real_t rand();
arr_real rand(int n);
arr_real rand(std::array<real_t, 2> range, int n);

//normally distributed random numbers
real_t randn();
arr_real randn(int n);

}   // namespace dsplib