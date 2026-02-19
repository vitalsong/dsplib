#include <dsplib/random.h>

#include <random>

namespace dsplib {

namespace {

//use global generator engine
DSPLIB_CACHE_T std::mt19937 g_engine{0};

}   // namespace

//-------------------------------------------------------------------------------------------------
void rng(int seed) {
    g_engine.seed(seed);
}

//-------------------------------------------------------------------------------------------------
int randi(int imax) {
    return randi({1, imax});
}

//-------------------------------------------------------------------------------------------------
arr_int randi(int imax, int n) {
    return randi({1, imax}, n);
}

//-------------------------------------------------------------------------------------------------
int randi(std::array<int, 2> range) {
    const int imin = range[0];
    const int imax = range[1];
    std::uniform_int_distribution<int> dist(imin, imax);
    return dist(g_engine);
}

//-------------------------------------------------------------------------------------------------
arr_int randi(std::array<int, 2> range, int n) {
    const int imin = range[0];
    const int imax = range[1];
    std::uniform_int_distribution<int> dist(imin, imax);
    arr_int r(n);
    for (int i = 0; i < n; ++i) {
        r[i] = dist(g_engine);
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real rand(int n) {
    arr_real r(n);
    std::uniform_real_distribution<real_t> dist{0, 1};
    for (int i = 0; i < n; ++i) {
        r[i] = dist(g_engine);
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
real_t rand() {
    std::uniform_real_distribution<real_t> dist{0, 1};
    return dist(g_engine);
}

//-------------------------------------------------------------------------------------------------
arr_real rand(std::array<real_t, 2> range, int n) {
    arr_real r(n);
    std::uniform_real_distribution<real_t> dist{range[0], range[1]};
    for (int i = 0; i < n; ++i) {
        r[i] = dist(g_engine);
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
arr_real randn(int n) {
    arr_real r(n);
    std::normal_distribution<real_t> dist{0, 1};
    for (int i = 0; i < n; ++i) {
        r[i] = dist(g_engine);
    }
    return r;
}

//-------------------------------------------------------------------------------------------------
real_t randn() {
    std::normal_distribution<real_t> dist{0, 1};
    return dist(g_engine);
}

}   // namespace dsplib