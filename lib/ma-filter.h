#pragma once

#include <dsplib/math.h>
#include <dsplib/array.h>

namespace dsplib {

/**
 * @brief Moving Average Filter
 * @details Implemented as a recurrent sum. 
 * The accumulator is recalculated every `n` points to eliminate numerical instability.
 * @todo Use binary tree
 */
template<typename T>
class MAFilter
{
public:
    explicit MAFilter(int n)
      : _buf(n)
      , _n{n} {
    }

    T process(const T& x) {
        _accum -= _buf[_pos];
        _accum += x;
        _buf[_pos] = x;
        _pos += 1;
        //reset accumulator (fix number unstability)
        if (_pos == _n) {
            _pos = 0;
            _accum = dsplib::sum(_buf);
        }
        return _accum / _n;
    }

    base_array<T> process(const base_array<T>& x) {
        base_array<T> y(x.size());
        for (int i = 0; i < x.size(); ++i) {
            y[i] = process(x[i]);
        }
        return y;
    }

    T operator()(const T& x) {
        return this->process(x);
    }

    base_array<T> operator()(const base_array<T>& x) {
        return this->process(x);
    }

private:
    base_array<T> _buf;
    int _n{0};
    int _pos{0};
    T _accum{0};
};

using MAFilterR = MAFilter<real_t>;
using MAFilterC = MAFilter<cmplx_t>;

}   // namespace dsplib