#ifndef MEDFILT_H
#define MEDFILT_H

#include <dsplib/array.h>

namespace dsplib {

//nth-order one-dimensional median filter
class medfilt
{
public:
    explicit medfilt(int n=3);
    arr_real filter(const arr_real& x);
    static arr_real process(arr_real& x, int n);

private:
    arr_real _d;    ///< delay
    arr_real _s;    ///< sorted delay
    int _i;         ///< delay cycle index
    int _n;         ///< delay size
};

}   ///< dsplib

#endif // MEDFILT_H
