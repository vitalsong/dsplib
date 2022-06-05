#pragma once

#include <dsplib/array.h>

namespace dsplib {

arr_real medfilt(arr_real& x, int n);

/*!
 * \brief Nth-order one-dimensional median filter
 */
class median_filter
{
public:
    explicit median_filter(int n = 3);
    arr_real process(const arr_real& x);

    arr_real operator()(const arr_real& x) {
        return this->process(x);
    }

private:
    arr_real _d;   ///< delay
    arr_real _s;   ///< sorted delay
    int _i;        ///< delay cycle index
    int _n;        ///< delay size
};

}   // namespace dsplib
