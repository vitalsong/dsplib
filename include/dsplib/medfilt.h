#ifndef MEDFILT_H
#define MEDFILT_H

#include <dsplib/array.h>

namespace dsplib {

arr_real medfilt(arr_real& x, int n);

/*!
 * \brief Nth-order one-dimensional median filter
 */
class median_filter
{
public:
    explicit median_filter(int n=3);
    arr_real process(const arr_real& x);

private:
    arr_real _d;    ///< delay
    arr_real _s;    ///< sorted delay
    int _i;         ///< delay cycle index
    int _n;         ///< delay size
};

}   ///< dsplib

#endif // MEDFILT_H
