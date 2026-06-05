#pragma once

#include <dsplib/array.h>
#include <utility>

namespace dsplib {

//Delay input signal by fixed samples
//TODO: add fractional delay
//TODO: rename to `FixedDelay`?
template<typename T>
class Delay
{
public:
    explicit Delay(int length)
      : nd_{length}
      , cbuf_(length) {
        DSPLIB_ASSERT(length >= 0, "Delay length must be non-negative");
    }

    explicit Delay(span_t<T> initial)
      : nd_{initial.size()}
      , cbuf_{initial} {
    }

    base_array<T> process(span_t<T> x) {
        base_array<T> dx(x);
        this->process(inplace(dx));
        return dx;
    }

    void process(inplace_span_t<T> ix) {
        if (nd_ == 0) {
            return;
        }
        auto x = ix.get();
        for (int i = 0; i < x.size(); ++i) {
            std::swap(cbuf_[pos_], x[i]);
            pos_ = (pos_ + 1) % nd_;
        }
    }

    base_array<T> operator()(span_t<T> x) {
        return this->process(x);
    }

    [[nodiscard]] int length() const noexcept {
        return nd_;
    }

private:
    int nd_{0};
    int pos_{0};
    base_array<T> cbuf_;
};

using DelayReal = Delay<real_t>;
using DelayCmplx = Delay<cmplx_t>;

}   // namespace dsplib