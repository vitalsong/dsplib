#pragma once

#include <dsplib/array.h>

namespace dsplib {

arr_real medfilt(arr_real& x, int n);

/*!
 * \brief Nth-order one-dimensional median filter
 * \details Use this filter for real-time data processing instead of medfilt function
 * \warning medfilt(x) is not equal MedianFilter result
 * medfilt({1, 2, 3, 4, 5}, 3) == {1, 2, 3, 4, 4}
 * MedianFilter flt(3); flt({1, 2, 3, 4, 5}) == {0, 1, 2, 3, 4}
 */
class MedianFilter
{
public:
    explicit MedianFilter(int n = 3, real_t init_value = 0);
    arr_real process(const arr_real& x);

    arr_real operator()(const arr_real& x) {
        return this->process(x);
    }

    [[nodiscard]] int order() const noexcept {
        return _n;
    }

private:
    arr_real _d;    ///< delay
    arr_real _s;    ///< sorted delay
    int _i;         ///< delay cycle index
    const int _n;   ///< delay size
};

using median_filter [[deprecated]] = MedianFilter;

}   // namespace dsplib
