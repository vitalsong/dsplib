#pragma once

#include <dsplib/array.h>

namespace dsplib {

arr_real medfilt(arr_real& x, int n);

/*!
 * \brief Nth-order one-dimensional median filter
 */
class MedianFilter
{
public:
    explicit MedianFilter(int n = 3);
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

using median_filter [[deprecated]] = MedianFilter;

}   // namespace dsplib
