#include "dft-tables.h"

#include <math.h>
#include <vector>
#include <map>
#include <mutex>

namespace dsplib {
namespace tables {

//static list with cached tables
static std::map <size_t, std::vector <cmplx_t>> g_tables;

//mt guard
static std::mutex g_mutex;

//-------------------------------------------------------------------------------------------------
static bool _cashed(size_t size)
{
    if (g_tables.find(size) == g_tables.end()) {
        return false;
    }
    else {
        return true;
    }
}

//-------------------------------------------------------------------------------------------------
static void _cash(size_t size)
{
    std::vector <cmplx_t> ms;
    ms.resize(size);

    double p;
    for (size_t i=0; i < size; ++i)
    {
        p = double(i) / size;
        ms[i].re = cos(2 * M_PI * p);
        ms[i].im = -sin(2 * M_PI * p);
    }

    g_tables.insert({size, ms});
}

//-------------------------------------------------------------------------------------------------
static void _uncash(size_t size)
{
    g_tables.erase(size);
}

//-------------------------------------------------------------------------------------------------
const cmplx_t* dft_table(size_t size)
{
    std::lock_guard <std::mutex> locker(g_mutex);

    if (!_cashed(size)) {
        _cash(size);
    }

    auto& m = g_tables.at(size);
    return m.data();
}

//-------------------------------------------------------------------------------------------------
void dft_cash(size_t size)
{
    std::lock_guard <std::mutex> locker(g_mutex);

    if (_cashed(size)) {
        return;
    }

    _cash(size);
}

//-------------------------------------------------------------------------------------------------
void dft_clear(size_t size)
{
    std::lock_guard <std::mutex> locker(g_mutex);

    if (!_cashed(size)) {
        return;
    }

    _uncash(size);
}

//-------------------------------------------------------------------------------------------------
bool dft_cashed(size_t size)
{
    std::lock_guard <std::mutex> locker(g_mutex);
    return _cashed(size);
}

}   ///< tables
}   ///< dsplib
