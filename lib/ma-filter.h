#pragma once

#include <dsplib/math.h>
#include <dsplib/array.h>

namespace dsplib {

/**
 * @brief Moving Average Filter
 * @details Implemented as a recurrent sum (via binary tree)
 */
template<typename T>
class MAFilter
{
public:
    explicit MAFilter(int len)
      : d_{nextpow2(len)}
      , w_{len} {
        buf_.resize((1 << (d_ + 1)));
    }

    explicit MAFilter(int len, const base_array<T>& init)
      : MAFilter{len} {
        DSPLIB_ASSERT(init.size() == w_, "`init` vector size must be equal window len");
        for (int i = 0; i < w_; i++) {
            buf_[(1 << d_) - 1 + i] = init[i];
        }
    }

    T process(const T& in) noexcept {
        buf_[(1 << d_) - 1 + pos_] = in;
        for (int k = (1 << d_) - 1 + pos_; k > 0;) {
            k = (k - 1) / 2;
            buf_[k] = buf_[2 * k + 1] + buf_[2 * k + 2];
        }
        pos_ = (pos_ + 1) % w_;
        return buf_[0] / w_;
    }

    base_array<T> process(const base_array<T>& in) noexcept {
        const int n = in.size();
        base_array<T> out(n);
        for (int i = 0; i < n; i++) {
            out[i] = process(in[i]);
        }
        return out;
    }

    T operator()(const T& x) noexcept {
        return this->process(x);
    }

    base_array<T> operator()(const base_array<T>& x) noexcept {
        return this->process(x);
    }

private:
    int d_;
    int w_;
    std::vector<T> buf_;
    int pos_{0};
};

using MAFilterR = MAFilter<real_t>;
using MAFilterC = MAFilter<cmplx_t>;

}   // namespace dsplib