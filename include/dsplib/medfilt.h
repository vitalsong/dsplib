#pragma once

#include <dsplib/array.h>

namespace dsplib {

arr_real medfilt(span_real x, int n);

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

    arr_real process(span_real x) noexcept;

    real_t process(const real_t& x) noexcept;

    arr_real operator()(span_real x) noexcept {
        return this->process(x);
    }

    real_t operator()(const real_t& x) noexcept {
        return this->process(x);
    }

    [[nodiscard]] int order() const noexcept {
        return _n;
    }

private:
    arr_real _d;   ///< delay
    arr_real _s;   ///< sorted delay
    int _i;        ///< delay cycle index
    int _n;        ///< delay size
};

}   // namespace dsplib
